/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Roberto Fernandez Bautista <roberto.fer.bau@gmail.com>
 * Copyright (C) 2020 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file cadstar_archive_parser.cpp
 * @brief Helper functions and common defines between schematic and PCB Archive files
 */

#include <plugins/cadstar/cadstar_archive_parser.h>


void CADSTAR_ARCHIVE_PARSER::EVALUE::Parse( XNODE* aNode )
{
    wxASSERT( aNode->GetName() == wxT( "E" ) );

    if( ( !GetXmlAttributeIDString( aNode, 0 ).ToLong( &Base ) )
            || ( !GetXmlAttributeIDString( aNode, 1 ).ToLong( &Exponent ) ) )
    {
        THROW_PARSING_IO_ERROR( wxT( "Base and Exponent" ),
                                wxString::Format( "%s->%s", aNode->GetParent()->GetName(),
                                                  aNode->GetParent()->GetName() ) );
    }
}


void CADSTAR_ARCHIVE_PARSER::POINT::Parse( XNODE* aNode )
{
    wxASSERT( aNode->GetName() == wxT( "PT" ) );

    x = GetXmlAttributeIDLong( aNode, 0 );
    y = GetXmlAttributeIDLong( aNode, 1 );
}




void CADSTAR_ARCHIVE_PARSER::LONGPOINT::Parse( XNODE* aNode )
{
    wxASSERT( aNode->GetName() == wxT( "PT" ) );

    x = GetXmlAttributeIDLong( aNode, 0 );
    y = GetXmlAttributeIDLong( aNode, 1 );
}


bool CADSTAR_ARCHIVE_PARSER::VERTEX::IsVertex( XNODE* aNode )
{
    wxString aNodeName = aNode->GetName();

    if( aNodeName == wxT( "PT" ) || aNodeName == wxT( "ACWARC" ) || aNodeName == wxT( "CWARC" )
            || aNodeName == wxT( "CWSEMI" ) || aNodeName == wxT( "ACWSEMI" ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


void CADSTAR_ARCHIVE_PARSER::VERTEX::Parse( XNODE* aNode )
{
    wxASSERT( IsVertex( aNode ) );

    wxString aNodeName = aNode->GetName();

    if( aNodeName == wxT( "PT" ) )
    {
        Type     = VERTEX_TYPE::POINT;
        Center.x = UNDEFINED_VALUE;
        Center.y = UNDEFINED_VALUE;
        End.Parse( aNode );
    }
    else if( aNodeName == wxT( "ACWARC" ) || aNodeName == wxT( "CWARC" ) )
    {
        if( aNodeName == wxT( "ACWARC" ) )
            Type = VERTEX_TYPE::ANTICLOCKWISE_ARC;
        else
            Type = VERTEX_TYPE::CLOCKWISE_ARC;

        std::vector<POINT> pts = ParseAllChildPoints( aNode, true, 2 );

        Center = pts[0];
        End    = pts[1];
    }
    else if( aNodeName == wxT( "ACWSEMI" ) || aNodeName == wxT( "CWSEMI" ) )
    {
        if( aNodeName == wxT( "ACWSEMI" ) )
            Type = VERTEX_TYPE::ANTICLOCKWISE_SEMICIRCLE;
        else
            Type = VERTEX_TYPE::CLOCKWISE_SEMICIRCLE;

        Center.x = UNDEFINED_VALUE;
        Center.y = UNDEFINED_VALUE;

        std::vector<POINT> pts = ParseAllChildPoints( aNode, true, 1 );

        End = pts[0];
    }
    else
    {
        wxASSERT_MSG( true, wxT( "Unknown VERTEX type" ) );
    }
}


double CADSTAR_ARCHIVE_PARSER::EVALUE::GetDouble()
{
    return Base * std::pow( 10.0, Exponent );
}


void CADSTAR_ARCHIVE_PARSER::InsertAttributeAtEnd( XNODE* aNode, wxString aValue )
{
    wxString result;
    int      numAttributes = 0;

    if( aNode->GetAttribute( wxT( "numAttributes" ), &result ) )
    {
        numAttributes = wxAtoi( result );
        aNode->DeleteAttribute( wxT( "numAttributes" ) );
        ++numAttributes;
    }

    aNode->AddAttribute( wxT( "numAttributes" ), wxString::Format( wxT( "%i" ), numAttributes ) );

    wxString paramName = wxT( "attr" );
    paramName << numAttributes;

    aNode->AddAttribute( paramName, aValue );
}


XNODE* CADSTAR_ARCHIVE_PARSER::LoadArchiveFile(
        const wxString& aFileName, const wxString& aFileTypeIdentifier )
{
    KEYWORD   emptyKeywords[1] = {};
    XNODE *   iNode = NULL, *cNode = NULL;
    int       tok;
    bool      cadstarFileCheckDone = false;
    wxString  str;
    wxCSConv  win1252( wxT( "windows-1252" ) );
    wxMBConv* conv = &win1252; // Initial testing suggests file encoding to be Windows-1252
                               // More samples required.
    FILE* fp = wxFopen( aFileName, wxT( "rt" ) );

    if( !fp )
        THROW_IO_ERROR( wxString::Format( _( "Cannot open file '%s'" ), aFileName ) );

    DSNLEXER lexer( emptyKeywords, 0, fp, aFileName );

    while( ( tok = lexer.NextTok() ) != DSN_EOF )
    {
        if( tok == DSN_RIGHT )
        {
            cNode = iNode;
            if( cNode )
            {
                iNode = cNode->GetParent();
            }
            else
            {
                //too many closing brackets
                THROW_IO_ERROR( _( "The selected file is not valid or might be corrupt!" ) );
            }
        }
        else if( tok == DSN_LEFT )
        {
            tok   = lexer.NextTok();
            str   = wxString( lexer.CurText(), *conv );
            cNode = new XNODE( wxXML_ELEMENT_NODE, str );

            if( iNode )
            {
                //we will add it as attribute as well as child node
                InsertAttributeAtEnd( iNode, str );
                iNode->AddChild( cNode );
            }
            else if( !cadstarFileCheckDone )
            {

                if( cNode->GetName() != aFileTypeIdentifier )
                    THROW_IO_ERROR( _( "The selected file is not valid or might be corrupt!" ) );

                cadstarFileCheckDone = true;
            }

            iNode = cNode;
        }
        else if( iNode )
        {
            str = wxString( lexer.CurText(), *conv );
            //Insert even if string is empty
            InsertAttributeAtEnd( iNode, str );
        }
        else
        {
            //not enough closing brackets
            THROW_IO_ERROR( _( "The selected file is not valid or might be corrupt!" ) );
        }
    }

    // Not enough closing brackets
    if( iNode != NULL )
        THROW_IO_ERROR( _( "The selected file is not valid or might be corrupt!" ) );

    // Throw if no data was parsed
    if( cNode )
        return cNode;
    else
        THROW_IO_ERROR( _( "The selected file is not valid or might be corrupt!" ) );

    return NULL;
}


bool CADSTAR_ARCHIVE_PARSER::IsValidAttribute( wxXmlAttribute* aAttribute )
{
    return aAttribute->GetName() != wxT( "numAttributes" );
}


wxString CADSTAR_ARCHIVE_PARSER::GetXmlAttributeIDString( XNODE* aNode, unsigned int aID )
{
    wxString attrName, retVal;
    attrName = "attr";
    attrName << aID;

    if( !aNode->GetAttribute( attrName, &retVal ) )
        THROW_MISSING_PARAMETER_IO_ERROR( std::to_string( aID ), aNode->GetName() );

    return retVal;
}


long CADSTAR_ARCHIVE_PARSER::GetXmlAttributeIDLong( XNODE* aNode, unsigned int aID )
{
    long retVal;

    if( !GetXmlAttributeIDString( aNode, aID ).ToLong( &retVal ) )
        THROW_PARSING_IO_ERROR( std::to_string( aID ), aNode->GetName() );

    return retVal;
}


void CADSTAR_ARCHIVE_PARSER::CheckNoChildNodes( XNODE* aNode )
{
    if( aNode && aNode->GetChildren() )
        THROW_UNKNOWN_NODE_IO_ERROR( aNode->GetChildren()->GetName(), aNode->GetName() );
}


void CADSTAR_ARCHIVE_PARSER::CheckNoNextNodes( XNODE* aNode )
{
    if( aNode && aNode->GetNext() )
        THROW_UNKNOWN_NODE_IO_ERROR( aNode->GetNext()->GetName(), aNode->GetParent()->GetName() );
}


void CADSTAR_ARCHIVE_PARSER::ParseChildEValue( XNODE* aNode, EVALUE& aValueToParse )
{
    if( aNode->GetChildren()->GetName() == wxT( "E" ) )
        aValueToParse.Parse( aNode->GetChildren() );
    else
        THROW_UNKNOWN_NODE_IO_ERROR( aNode->GetChildren()->GetName(), aNode->GetName() );
}


std::vector<CADSTAR_ARCHIVE_PARSER::POINT> CADSTAR_ARCHIVE_PARSER::ParseAllChildPoints(
        XNODE* aNode, bool aTestAllChildNodes, int aExpectedNumPoints )
{
    std::vector<POINT> retVal;

    XNODE* cNode = aNode->GetChildren();

    for( ; cNode; cNode = cNode->GetNext() )
    {
        if( cNode->GetName() == wxT( "PT" ) )
        {
            POINT pt;
            //TODO try.. catch + throw again with more detailed error information
            pt.Parse( cNode );
            retVal.push_back( pt );
        }
        else if( aTestAllChildNodes )
        {
            THROW_UNKNOWN_NODE_IO_ERROR( cNode->GetName(), aNode->GetName() );
        }
    }

    if( aExpectedNumPoints != UNDEFINED_VALUE &&
        retVal.size() != static_cast<size_t>( aExpectedNumPoints ) )
    {
        THROW_IO_ERROR( wxString::Format(
                        _( "Unexpected number of points in '%s'. Found %d but expected %d." ),
                        aNode->GetName(), retVal.size(), aExpectedNumPoints ) );
    }

    return retVal;
}


std::vector<CADSTAR_ARCHIVE_PARSER::VERTEX> CADSTAR_ARCHIVE_PARSER::ParseAllChildVertices(
        XNODE* aNode, bool aTestAllChildNodes )
{
    std::vector<VERTEX> retVal;

    XNODE* cNode = aNode->GetChildren();

    for( ; cNode; cNode = cNode->GetNext() )
    {
        if( VERTEX::IsVertex( cNode ) )
        {
            VERTEX vertex;
            //TODO try.. catch + throw again with more detailed error information
            vertex.Parse( cNode );
            retVal.push_back( vertex );
        }
        else if( aTestAllChildNodes )
        {
            THROW_UNKNOWN_NODE_IO_ERROR( cNode->GetName(), aNode->GetName() );
        }
    }

    return retVal;
}


std::vector<CADSTAR_ARCHIVE_PARSER::CUTOUT> CADSTAR_ARCHIVE_PARSER::ParseAllChildCutouts(
        XNODE* aNode, bool aTestAllChildNodes )
{
    std::vector<CUTOUT> retVal;

    XNODE* cNode = aNode->GetChildren();

    for( ; cNode; cNode = cNode->GetNext() )
    {
        if( cNode->GetName() == wxT( "CUTOUT" ) )
        {
            CUTOUT cutout;
            //TODO try.. catch + throw again with more detailed error information
            cutout.Parse( cNode );
            retVal.push_back( cutout );
        }
        else if( aTestAllChildNodes )
        {
            THROW_UNKNOWN_NODE_IO_ERROR( cNode->GetName(), aNode->GetName() );
        }
    }

    return retVal;
}


void CADSTAR_ARCHIVE_PARSER::CUTOUT::Parse( XNODE* aNode )
{
    wxASSERT( aNode->GetName() == wxT( "CUTOUT" ) );

    Vertices = ParseAllChildVertices( aNode, true );
}


bool CADSTAR_ARCHIVE_PARSER::SHAPE::IsShape( XNODE* aNode )
{
    wxString aNodeName = aNode->GetName();

    if( aNodeName == wxT( "OPENSHAPE" ) || aNodeName == wxT( "OUTLINE" )
            || aNodeName == wxT( "SOLID" ) || aNodeName == wxT( "HATCHED" ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


void CADSTAR_ARCHIVE_PARSER::SHAPE::Parse( XNODE* aNode )
{
    wxASSERT( IsShape( aNode ) );

    wxString aNodeName = aNode->GetName();

    if( aNodeName == wxT( "OPENSHAPE" ) )
    {
        Type     = SHAPE_TYPE::OPENSHAPE;
        Vertices = ParseAllChildVertices( aNode, true );
        Cutouts.clear();
        HatchCodeID = wxEmptyString;
    }
    else if( aNodeName == wxT( "OUTLINE" ) )
    {
        Type        = SHAPE_TYPE::OUTLINE;
        Vertices    = ParseAllChildVertices( aNode, false );
        Cutouts     = ParseAllChildCutouts( aNode, false );
        HatchCodeID = wxEmptyString;
    }
    else if( aNodeName == wxT( "SOLID" ) )
    {
        Type        = SHAPE_TYPE::SOLID;
        Vertices    = ParseAllChildVertices( aNode, false );
        Cutouts     = ParseAllChildCutouts( aNode, false );
        HatchCodeID = wxEmptyString;
    }
    else if( aNodeName == wxT( "HATCHED" ) )
    {
        Type        = SHAPE_TYPE::HATCHED;
        Vertices    = ParseAllChildVertices( aNode, false );
        Cutouts     = ParseAllChildCutouts( aNode, false );
        HatchCodeID = GetXmlAttributeIDString( aNode, 0 );
    }
    else
    {
        wxASSERT_MSG( true, wxT( "Unknown SHAPE type" ) );
    }
}
