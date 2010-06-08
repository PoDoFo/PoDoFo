/***************************************************************************
 *   Copyright (C) 2008 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "EncryptTest.h"
#include "TestUtils.h"
#include <podofo.h>

#include <stdlib.h>

using namespace PoDoFo;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EncryptTest );

void EncryptTest::setUp()
{
    const char* pBuffer1 = "Somekind of drawing \001 buffer that possibly \003 could contain PDF drawing commands";
    const char* pBuffer2 = " possibly could contain PDF drawing\003  commands";
    
    m_lLen       = strlen( pBuffer1 ) + 2 * strlen( pBuffer2 );
    m_pEncBuffer = static_cast<char*>(malloc( sizeof(char) * m_lLen ));

    memcpy( m_pEncBuffer, pBuffer1, strlen( pBuffer1 ) * sizeof(char) );
    memcpy( m_pEncBuffer + strlen(pBuffer1), pBuffer2, strlen( pBuffer2 ) );
    memcpy( m_pEncBuffer + strlen(pBuffer1) + strlen( pBuffer2 ), pBuffer2, strlen( pBuffer2 ) );

    m_protection = PdfEncrypt::ePdfPermissions_Print | 
        PdfEncrypt::ePdfPermissions_Edit |
        PdfEncrypt::ePdfPermissions_Copy |
        PdfEncrypt::ePdfPermissions_EditNotes | 
        PdfEncrypt::ePdfPermissions_FillAndSign |
        PdfEncrypt::ePdfPermissions_Accessible |
        PdfEncrypt::ePdfPermissions_DocAssembly |
        PdfEncrypt::ePdfPermissions_HighPrint;

}

void EncryptTest::tearDown()
{
    free( m_pEncBuffer );
}

void EncryptTest::testDefault() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo" );

    TestAuthenticate( pEncrypt, 40, 2 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testRC4() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V1, 
                                                         PdfEncrypt::ePdfKeyLength_40 );

    TestAuthenticate( pEncrypt, 40, 2 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testRC4v2_40() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V2, 
                                                         PdfEncrypt::ePdfKeyLength_40 );

    TestAuthenticate( pEncrypt, 40, 3 );
    TestEncrypt( pEncrypt );


    delete pEncrypt;
}

void EncryptTest::testRC4v2_56() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V2, 
                                                         PdfEncrypt::ePdfKeyLength_56 );

    TestAuthenticate( pEncrypt, 56, 3 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testRC4v2_80() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V2, 
                                                         PdfEncrypt::ePdfKeyLength_80 );

    TestAuthenticate( pEncrypt, 80, 3 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testRC4v2_96() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V2, 
                                                         PdfEncrypt::ePdfKeyLength_96 );

    TestAuthenticate( pEncrypt, 96, 3 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testRC4v2_128() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_RC4V2, 
                                                         PdfEncrypt::ePdfKeyLength_128 );

    TestAuthenticate( pEncrypt, 128, 3 );
    TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::testAES() 
{
    PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( "user", "podofo", m_protection, 
                                                         PdfEncrypt::ePdfEncryptAlgorithm_AESV2, 
                                                         PdfEncrypt::ePdfKeyLength_128 );

    TestAuthenticate( pEncrypt, 128, 4 );
    // AES decryption is not yet implemented.
    // Therefore we have to disable this test.
    // TestEncrypt( pEncrypt );

    delete pEncrypt;
}

void EncryptTest::TestAuthenticate( PdfEncrypt* pEncrypt, int keyLength, int rValue ) 
{
    PdfString documentId;
    documentId.SetHexData( "BF37541A9083A51619AD5924ECF156DF", 32 );

    pEncrypt->GenerateEncryptionKey( documentId );

    std::string documentIdStr( documentId.GetString(), documentId.GetLength() );
    std::string password = "user";
    std::string uValue( reinterpret_cast<const char*>(pEncrypt->GetUValue()), 32 );
    std::string oValue( reinterpret_cast<const char*>(pEncrypt->GetOValue()), 32 );

    // Authenticate using user password
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "authenticate using user password",
                                  pEncrypt->Authenticate( documentIdStr, std::string("user"),
                                                          uValue, oValue, pEncrypt->GetPValue(), keyLength, rValue ),
                                  true );

    // Authenticate using owner password
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "authenticate using owner password",
                                  pEncrypt->Authenticate( documentIdStr, std::string("podofo"), 
                                                          uValue, oValue, pEncrypt->GetPValue(), keyLength, rValue ),
                                  true );
}

void EncryptTest::TestEncrypt( PdfEncrypt* pEncrypt ) 
{
    pEncrypt->SetCurrentReference( PdfReference( 7, 0 ) );
    
    int nOutputLen = pEncrypt->CalculateStreamLength(m_lLen);
    int nOffset = pEncrypt->CalculateStreamOffset();

    char *pOutputBuffer = new char[nOutputLen+1];
    memcpy(&pOutputBuffer[nOffset], m_pEncBuffer, m_lLen);

    // Encrypt buffer
    pEncrypt->Encrypt( reinterpret_cast<unsigned char*>(pOutputBuffer), m_lLen );

    // Decrypt buffer
    pEncrypt->Encrypt( reinterpret_cast<unsigned char*>(pOutputBuffer), m_lLen );


    CPPUNIT_ASSERT_EQUAL_MESSAGE( "compare encrypted and decrypted buffer",
                                  0, memcmp( m_pEncBuffer, pOutputBuffer, m_lLen ) );

    delete[] pOutputBuffer;
}

void EncryptTest::testLoadEncrypedFilePdfParser()
{
    std::string sFilename = TestUtils::getTempFilename();

    try {
        CreatedEncrypedPdf( sFilename.c_str() );
    
        // Try loading with PdfParser
        PdfVecObjects objects;
        PdfParser     parser( &objects );

        try {
            parser.ParseFile( sFilename.c_str(), true );

            // Must throw an exception
            CPPUNIT_FAIL("Encrypted file not recognized!");
        } catch( const PdfError & e ) {
            if( e.GetError() != ePdfError_InvalidPassword ) 
            {
                CPPUNIT_FAIL("Invalid encryption exception thrown!");
            }
        }

        parser.SetPassword( "user" );
    } catch( PdfError & e ) {
        e.PrintErrorMsg();

        printf("Removing temp file: %s\n", sFilename.c_str());
        TestUtils::deleteFile(sFilename.c_str());
        throw e;
    }

    printf("Removing temp file: %s\n", sFilename.c_str());
    TestUtils::deleteFile(sFilename.c_str());
}

void EncryptTest::testLoadEncrypedFilePdfMemDocument()
{
    std::string sFilename = TestUtils::getTempFilename();

    try {
        CreatedEncrypedPdf( sFilename.c_str() );
    
        // Try loading with PdfParser
        PdfMemDocument document;
        try {
            document.Load( sFilename.c_str() );

            // Must throw an exception
            CPPUNIT_FAIL("Encrypted file not recognized!");
        } catch( const PdfError & e ) {
            if( e.GetError() != ePdfError_InvalidPassword ) 
            {
                CPPUNIT_FAIL("Invalid encryption exception thrown!");
            }
        }
        
        document.SetPassword( "user" );

    } catch( PdfError & e ) {
        e.PrintErrorMsg();

        printf("Removing temp file: %s\n", sFilename.c_str());
        TestUtils::deleteFile(sFilename.c_str());

        throw e;
    }

    printf("Removing temp file: %s\n", sFilename.c_str());
    TestUtils::deleteFile(sFilename.c_str());
}

void EncryptTest::CreatedEncrypedPdf( const char* pszFilename )
{
    PdfMemDocument  writer;
    PdfPage* pPage = writer.CreatePage( PdfPage::CreateStandardPageSize( ePdfPageSize_A4 ) );
    PdfPainter painter;
    painter.SetPage( pPage );

    PdfFont* pFont = writer.CreateFont( "Arial", PdfEncodingFactory::GlobalWinAnsiEncodingInstance(), false );
    if( !pFont )
    {
        PODOFO_RAISE_ERROR( ePdfError_InvalidHandle );
    }

    pFont->SetFontSize( 16.0 );
    painter.SetFont( pFont );
    painter.DrawText( 100, 100, "Hello World" );
    painter.FinishPage();

    writer.SetEncrypted( "user", "owner" );
    writer.Write( pszFilename );

    printf( "Wrote: %s\n", pszFilename );
}

void EncryptTest::testEnableAlgorithms()
{
    int nDefault = PdfEncrypt::GetEnabledEncryptionAlgorithms();

    // By default every algorithms should be enabled
    CPPUNIT_ASSERT( PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_RC4V1 ) );
    CPPUNIT_ASSERT( PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_RC4V2 ) );
    CPPUNIT_ASSERT( PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_AESV2 ) );

    CPPUNIT_ASSERT_EQUAL( PdfEncrypt::ePdfEncryptAlgorithm_RC4V1 |
                          PdfEncrypt::ePdfEncryptAlgorithm_RC4V2 |
                          PdfEncrypt::ePdfEncryptAlgorithm_AESV2,
                          PdfEncrypt::GetEnabledEncryptionAlgorithms() );
    // Disable AES
    PdfEncrypt::SetEnabledEncryptionAlgorithms( PdfEncrypt::ePdfEncryptAlgorithm_RC4V1 |
                                                PdfEncrypt::ePdfEncryptAlgorithm_RC4V2 );

    CPPUNIT_ASSERT( PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_RC4V1 ) );
    CPPUNIT_ASSERT( PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_RC4V2 ) );
    CPPUNIT_ASSERT( !PdfEncrypt::IsEncryptionEnabled( PdfEncrypt::ePdfEncryptAlgorithm_AESV2 ) );

    CPPUNIT_ASSERT_EQUAL( PdfEncrypt::ePdfEncryptAlgorithm_RC4V1 |
                          PdfEncrypt::ePdfEncryptAlgorithm_RC4V2,
                          PdfEncrypt::GetEnabledEncryptionAlgorithms() );


    PdfObject object;
    object.GetDictionary().AddKey(PdfName("Filter"), PdfName("Standard"));
    object.GetDictionary().AddKey(PdfName("V"), static_cast<pdf_int64>(4L));
    object.GetDictionary().AddKey(PdfName("R"), static_cast<pdf_int64>(4L));
    object.GetDictionary().AddKey(PdfName("P"), static_cast<pdf_int64>(1L));
    object.GetDictionary().AddKey(PdfName("O"), PdfString(""));
    object.GetDictionary().AddKey(PdfName("U"), PdfString(""));

    try {
        PdfEncrypt* pEncrypt = PdfEncrypt::CreatePdfEncrypt( &object );
        CPPUNIT_ASSERT( false );
    } catch( PdfError & rError ) {
        CPPUNIT_ASSERT_EQUAL( rError.GetError(), ePdfError_UnsupportedFilter );
    }

    // Restore default
    PdfEncrypt::SetEnabledEncryptionAlgorithms( nDefault );
}


                                  /*


    PdfMemoryOutputStream mem( lLen );
    PdfOutputStream* pStream = enc.CreateEncryptionOutputStream( &mem ); 
    pStream->Write( pBuffer1, strlen( pBuffer1 ) );
    pStream->Write( pBuffer2, strlen( pBuffer2 ) );
    pStream->Write( pBuffer2, strlen( pBuffer2 ) );
    pStream->Close();

    printf("Result: %i \n", memcmp( pEncBuffer, mem.TakeBuffer(), lLen ) );


    enc.Encrypt( reinterpret_cast<unsigned char*>(pEncBuffer), lLen );
    printf("Decrypted buffer: %s\n", pEncBuffer );
    */

