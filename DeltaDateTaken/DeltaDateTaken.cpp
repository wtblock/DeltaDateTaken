/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeltaDateTaken.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
// The date and time when the original image data was generated.
// For a digital still camera, this is the date and time the picture 
// was taken or recorded. The format is "YYYY:MM:DD HH:MM:SS" with time 
// shown in 24-hour format, and the date and time separated by one blank 
// character (hex 20).
void CDate::SetDateTaken( CString csDate )
{
	// reset the m_Date to undefined state
	Year = -1;
	Month = -1;
	Day = -1;
	Hour = 0;
	Minute = 0;
	Second = 0;
	bool value = Okay;

	// parse the date into a vector of string tokens
	const CString csDelim( _T( ": " ) );
	int nStart = 0;
	vector<CString> tokens;

	do
	{
		const CString csToken =
			csDate.Tokenize( csDelim, nStart ).MakeLower();
		if ( csToken.IsEmpty() )
		{
			break;
		}

		tokens.push_back( csToken );

	} while ( true );

	// there should be six tokens in the proper format of
	// "YYYY:MM:DD HH:MM:SS"
	const size_t tTokens = tokens.size();
	if ( tTokens != 6 )
	{
		return;
	}

	// populate the date and time members with the values
	// in the vector
	TOKEN_NAME eToken = tnYear;
	int nToken = 0;

	for ( CString csToken : tokens )
	{
		int nValue = _tstol( csToken );

		switch ( eToken )
		{
			case tnYear:
			{
				Year = nValue;
				break;
			}
			case tnMonth:
			{
				Month = nValue;
				break;
			}
			case tnDay:
			{
				Day = nValue;
				break;
			}
			case tnHour:
			{
				Hour = nValue;
				break;
			}
			case tnMinute:
			{
				Minute = nValue;
				break;
			}
			case tnSecond:
			{
				Second = nValue;
				break;
			}
		}

		nToken++;
		eToken = (TOKEN_NAME)nToken;
	}

	// this will be true if all of the values define a proper date and time
	value = Okay;

} // SetDateTaken

/////////////////////////////////////////////////////////////////////////////
// given an image pointer and an ASCII property ID, return the property value
CString GetStringProperty( Gdiplus::Image* pImage, PROPID id )
{
	CString value;

	// get the size of the date property
	const UINT uiSize = pImage->GetPropertyItemSize( id );

	// if the property exists, it will have a non-zero size 
	if ( uiSize > 0 )
	{
		// using a smart pointer which will release itself
		// when it goes out of context
		unique_ptr<Gdiplus::PropertyItem> pItem =
			unique_ptr<Gdiplus::PropertyItem>
			(
				(PropertyItem*)malloc( uiSize )
			);

		// Get the property item.
		pImage->GetPropertyItem( id, uiSize, pItem.get() );

		// the property should be ASCII
		if ( pItem->type == PropertyTagTypeASCII )
		{
			value = (LPCSTR)pItem->value;
		}
	}

	return value;
} // GetStringProperty

/////////////////////////////////////////////////////////////////////////////
// get the current date taken, if any, from the given filename
// which should be in the format "YYYY:MM:DD HH:MM:SS"
CString GetCurrentDateTaken( LPCTSTR lpszPathName )
{
	USES_CONVERSION;

	CString value;

	// smart pointer to the image representing this file
	// (smart pointer release their resources when they
	// go out of context)
	unique_ptr<Gdiplus::Image> pImage =
		unique_ptr<Gdiplus::Image>
		(
			Gdiplus::Image::FromFile( T2CW( lpszPathName ) )
		);

	// test the date properties stored in the given image
	const CString csOriginal =
		GetStringProperty( pImage.get(), PropertyTagExifDTOrig );
	const CString csDigitized =
		GetStringProperty( pImage.get(), PropertyTagExifDTDigitized );

	// officially the original property is the date taken in this
	// format: "YYYY:MM:DD HH:MM:SS"
	m_Date.DateTaken = csOriginal;
	if ( m_Date.Okay )
	{
		value = csOriginal;

	} else // alternately use the date digitized
	{
		m_Date.DateTaken = csDigitized;
		if ( m_Date.Okay )
		{
			value = csDigitized;
		}
	}

	return value;
} // GetCurrentDateTaken

/////////////////////////////////////////////////////////////////////////////
COleDateTime GetDateAndTime
( 
	CString csPath, // path to the image file being tested
	CString csDesc, // description of the image file being tested
	CStdioFile& fout // console output device
)
{
	COleDateTime value;

	// read the current date and time from the metadata
	// which should be in this format from the image file
	// "YYYY:MM:DD HH:MM:SS"
	const CString csDateTaken = GetCurrentDateTaken( csPath );

	// if the date taken is empty, there is nothing for us
	// to do
	CString csOutput;
	if ( csDateTaken.IsEmpty() )
	{
		csOutput.Format
		(
			_T( "%s is missing.\n" ), csDesc
		);
		fout.WriteString( _T( ".\n" ) );
		fout.WriteString( csOutput );
		fout.WriteString( _T( ".\n" ) );
		return value;
	}

	m_Date.DateTaken = csDateTaken;
	const bool bValid = m_Date.Okay;
	if ( !bValid )
	{
		csOutput.Format
		(
			_T( "%s: %s is invalid.\n" ), csDateTaken, csDesc
		);
		fout.WriteString( _T( ".\n" ) );
		fout.WriteString( csOutput );
		fout.WriteString( _T( ".\n" ) );
		return value;

	} else
	{
		csOutput.Format
		(
			_T( "%s: %s is valid.\n" ), csDateTaken, csDesc
		);
		fout.WriteString( csOutput );

		// get the date and time from the m_Date member class
		value = m_Date.DateAndTime;
	}

	return value;
} // GetDateAndTime

/////////////////////////////////////////////////////////////////////////////
// a console application that can crawl through the file
// system and troll for image metadata properties
int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	HMODULE hModule = ::GetModuleHandle( NULL );
	if ( hModule == NULL )
	{
		_tprintf( _T( "Fatal Error: GetModuleHandle failed\n" ) );
		return 1;
	}

	// initialize MFC and error on failure
	if ( !AfxWinInit( hModule, NULL, ::GetCommandLine(), 0 ) )
	{
		_tprintf( _T( "Fatal Error: MFC initialization failed\n " ) );
		return 2;
	}

	CStdioFile fOut( stdout );

	// if the expected number of parameters are not found
	// give the user some usage information
	if ( argc != 3 )
	{
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString
		(
			_T( "DeltaDateTaken, Copyright (c) 2020, " )
			_T( "by W. T. Block.\n" )
		);
		fOut.WriteString
		( 
			_T( ".\n" ) 
			_T( "Usage:\n" )
			_T( ".\n" )
			_T( ".  DeltaDateTaken ref_path offset_path \n" )
			_T( ".\n" )
			_T( "Where:\n" )
			_T( ".\n" )
		);

		fOut.WriteString
		(
			_T( ".  ref_path is the reference pathname to the file\n" )
			_T( ".    with the correct date taken meta-data.\n" )
		);

		fOut.WriteString
		(
			_T( ".  offset_path is the pathname to the file with the\n" )
			_T( ".    date taken meta-data that is offset from the date\n" )
			_T( ".    taken in the reference path.\n" )
			_T( ".\n" )
		);

		fOut.WriteString
		( 
			_T( "Displays the number of hours between the\n" ) 
			_T( "  the two date taken meta-data values.\n" )
			_T( ".\n" )
			_T( "Where:\n" )
			_T( ".\n" )
			_T( ".  a positive value indicates the reference date\n" )
			_T( ".    taken is greater than the offset date taken.\n" )
			_T( ".  a negative value indicates the reference date\n" )
			_T( ".    taken is less than the offset date taken.\n" )
			_T( ".\n" )
		);
		return 3;
	}

	// display the executable path
	CString csMessage;
	//csMessage.Format( _T( "Executable pathname: %s\n" ), argv[ 0 ] );
	//fOut.WriteString( _T( ".\n" ) );
	//fOut.WriteString( csMessage );
	//fOut.WriteString( _T( ".\n" ) );

	// retrieve the reference pathname
	const CString csPathRef = argv[ 1 ];

	// insure the given reference path exists
	const bool bExistsRef = FALSE != ::PathFileExists( csPathRef );

	// give feedback to the user
	if ( !bExistsRef )
	{
		csMessage.Format
		( 
			_T( "Invalid reference pathname:\n\t%s\n" ), csPathRef 
		);
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString( csMessage );
		fOut.WriteString( _T( ".\n" ) );
		return 4;

	} else // reference path exists
	{
		csMessage.Format
		( 
			_T( "Given reference pathname:\n\t%s\n" ), csPathRef 
		);
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString( csMessage );
	}

	// retrieve the offset pathname
	const CString csPathOff = argv[ 2 ];

	// insure the given offset path exists
	const bool bExistsOff = FALSE != ::PathFileExists( csPathOff );

	// give feedback to the user
	if ( !bExistsOff )
	{
		csMessage.Format
		( 
			_T( "Invalid offset pathname:\n\t%s\n" ), csPathOff 
		);
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString( csMessage );
		fOut.WriteString( _T( ".\n" ) );
		return 4;

	} else // offset path exists
	{
		csMessage.Format
		( 
			_T( "Given offset pathname:\n\t%s\n" ), csPathOff 
		);
		fOut.WriteString( csMessage );
		fOut.WriteString( _T( ".\n" ) );
	}

	int value = 0;

	// start up COM
	AfxOleInit();
	::CoInitialize( NULL );

	// reference to GDI+
	InitGdiplus();

	// lookup the date taken from the reference path
	COleDateTime oRef = GetDateAndTime
	( 
		csPathRef, _T( "Reference Date Taken" ), fOut 
	);

	// test for a good status of the reference date
	const COleDateTime::DateTimeStatus eRef = oRef.GetStatus();

	if ( eRef == COleDateTime::valid )
	{
		// lookup the date taken from the offset path
		COleDateTime oOff = GetDateAndTime
		(
			csPathOff, _T( "Offset Date Taken" ), fOut
		);

		// test for a good status of the offset date
		const COleDateTime::DateTimeStatus eOff = oOff.GetStatus();

		if ( eOff == COleDateTime::valid )
		{
			// get the time span between the two dates
			COleDateTimeSpan oSpan( oRef - oOff );

			// test for a valid time span
			COleDateTimeSpan::DateTimeSpanStatus eSpan =
				oSpan.GetStatus();

			if ( eSpan == COleDateTimeSpan::valid )
			{
				// the span is stored in days
				m_dHourOffset = oSpan.m_span;

				// convert to hours
				m_dHourOffset *= 24;

				// tell the user what the offset is
				csMessage.Format
				(
					_T( "The Date Taken difference is: %f hours.\n" ), 
					m_dHourOffset
				);
				fOut.WriteString( csMessage );
				fOut.WriteString( _T( ".\n" ) );

			} else // time span is invalid
			{
				fOut.WriteString( _T( ".\n" ) );
				fOut.WriteString
				( 
					_T( "The time span failed to calculate.\n" ) 
				);
				fOut.WriteString( _T( ".\n" ) );
				value = 7;
			}
		} else // offset is not valid
		{
			value = 6;
		}
	} else // reference is not valid
	{
		value = 5;
	}

	// clean up references to GDI+
	TerminateGdiplus();

	// let batch files know how we did so they can
	// take appropriate action
	return value;

} // _tmain

/////////////////////////////////////////////////////////////////////////////
