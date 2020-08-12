/////////////////////////////////////////////////////////////////////////////
// Copyright � by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "resource.h"
#include "KeyedCollection.h"
#include <comutil.h>
#include <vector>
#include <map>
#include <memory>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib" )
#else
#pragma comment(lib, "comsuppw.lib" )
#endif

using namespace Gdiplus;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// this class records the date and time information in each image file 
// referenced
class CDate
{
	// protected definition
protected:
	typedef struct tagMonthLookup
	{
		CString m_csMonth;
		int m_nMonth;

	} MONTH_LOOKUP;

	typedef enum
	{
		tnYear = 0,
		tnMonth = tnYear + 1,
		tnDay = tnMonth + 1,
		tnHour = tnDay + 1,
		tnMinute = tnHour + 1,
		tnSecond = tnMinute + 1,
	} TOKEN_NAME;

	// protected data
protected:
	// date formatted as a string
	CString m_csDate;

	// The date and time when the original image data was generated.
	// For a digital still camera, this is the date and time the picture 
	// was taken or recorded. The format is "YYYY:MM:DD HH:MM:SS" with time 
	// shown in 24-hour format, and the date and time separated by one blank 
	// character (hex 20).
	CString m_csDateTaken;

	// 4 digit year
	int m_nYear;

	// month as a number (1..12)
	int m_nMonth;

	// day of the month (0..31)
	int m_nDay;

	// hour of the day (0..23)
	int m_nHour;

	// minute of the hour (0..59)
	int m_nMinute;

	// second of the minute (0..59)
	int m_nSecond;

	// boolean indicator that all is well
	bool m_bOkay;

	// rapid month lookup
	CKeyedCollection<CString, int> m_MonthLookup;

	// public properties
public:
	// date and time formatted as a string
	inline CString GetDate()
	{
		CString value;
		COleDateTime oDT = DateAndTime;
		COleDateTime::DateTimeStatus eStatus = oDT.GetStatus();
		bool bOkay = COleDateTime::DateTimeStatus::valid == eStatus;

		// if the status is good, format into a string
		if ( bOkay )
		{
			value = oDT.Format( _T( "%Y:%m:%d %H:%M:%S" ) );
			m_csDate = value;
		}

		return m_csDate;
	}
	// date and time formatted as a string
	inline void SetDate( CString value )
	{
		COleDateTime oDT;
		if ( oDT.ParseDateTime( value ) )
		{
			DateAndTime = oDT;
			Okay = true;
			m_csDate = value;
		}
	}
	// date and time formatted as a string
	__declspec( property( get = GetDate, put = SetDate ) )
		CString Date;

	// 4 digit year
	inline int GetYear()
	{
		return m_nYear;
	}
	// 4 digit year
	inline void SetYear( int value )
	{
		m_nYear = value;
	}
	// 4 digit year
	__declspec( property( get = GetYear, put = SetYear ) )
		int Year;

	// month of the year as a number (1..12)
	inline int GetMonth()
	{
		return m_nMonth;
	}
	// month of the year as a number (1..12)
	inline void SetMonth( int value )
	{
		m_nMonth = value;
	}
	// month of the year as a number (1..12)
	__declspec( property( get = GetMonth, put = SetMonth ) )
		int Month;

	// day of the month (0..31)
	inline int GetDay()
	{
		return m_nDay;
	}
	// day of the month (0..31)
	inline void SetDay( int value )
	{
		m_nDay = value;
	}
	// day of the month (0..31)
	__declspec( property( get = GetDay, put = SetDay ) )
		int Day;

	// hour of the day (0..23)
	inline int GetHour()
	{
		return m_nHour;
	}
	// hour of the day (0..23)
	inline void SetHour( int value )
	{
		m_nHour = value;
	}
	// hour of the day (0..23)
	__declspec( property( get = GetHour, put = SetHour ) )
		int Hour;

	// minute of the hour (0..59)
	inline int GetMinute()
	{
		return m_nMinute;
	}
	// minute of the hour (0..59)
	inline void SetMinute( int value )
	{
		m_nMinute = value;
	}
	// minute of the hour (0..59)
	__declspec( property( get = GetMinute, put = SetMinute ) )
		int Minute;

	// second of the minute (0..59)
	inline int GetSecond()
	{
		return m_nSecond;
	}
	// second of the minute (0..59)
	inline void SetSecond( int value )
	{
		m_nSecond = value;
	}
	// second of the minute (0..59)
	__declspec( property( get = GetSecond, put = SetSecond ) )
		int Second;

	// boolean indicator that all is well
	inline bool GetOkay()
	{
		COleDateTime oDT( Year, Month, Day, Hour, Minute, Second );
		COleDateTime::DateTimeStatus eStatus = oDT.GetStatus();
		Okay = COleDateTime::DateTimeStatus::valid == eStatus;
		return m_bOkay;
	}
	// boolean indicator that all is well
	inline void SetOkay( bool value )
	{
		m_bOkay = value;
	}
	// boolean indicator that all is well
	__declspec( property( get = GetOkay, put = SetOkay ) )
		bool Okay;

	// gets the date and time from the properties
	inline COleDateTime GetDateAndTime()
	{
		COleDateTime value( Year, Month, Day, Hour, Minute, Second );
		COleDateTime::DateTimeStatus eStatus = value.GetStatus();
		Okay = COleDateTime::DateTimeStatus::valid == eStatus;
		return value;
	}
	// sets the date and time if valid
	inline void SetDateAndTime( COleDateTime value )
	{
		COleDateTime::DateTimeStatus eStatus = value.GetStatus();
		bool bOkay = COleDateTime::DateTimeStatus::valid == eStatus;
		if ( bOkay )
		{
			Year = value.GetYear();
			Month = value.GetMonth();
			Day = value.GetDay();
			Hour = value.GetHour();
			Minute = value.GetMinute();
			Second = value.GetSecond();
			Okay = bOkay;
		}
	}
	// date and time property
	__declspec( property( get = GetDateAndTime, put = SetDateAndTime ) )
		COleDateTime DateAndTime;

	// The date and time when the original image data was generated.
	// For a digital still camera, this is the date and time the picture 
	// was taken or recorded. The format is "YYYY:MM:DD HH:MM:SS" with time 
	// shown in 24-hour format, and the date and time separated by one blank 
	// character (hex 20).
	inline CString GetDateTaken()
	{
		return m_csDateTaken;
	}
	// The date and time when the original image data was generated.
	// For a digital still camera, this is the date and time the picture 
	// was taken or recorded. The format is "YYYY:MM:DD HH:MM:SS" with time 
	// shown in 24-hour format, and the date and time separated by one blank 
	// character (hex 20).
	void SetDateTaken( CString csDate );
	// The date and time when the original image data was generated.
	// For a digital still camera, this is the date and time the picture 
	// was taken or recorded. The format is "YYYY:MM:DD HH:MM:SS" with time 
	// shown in 24-hour format, and the date and time separated by one blank 
	// character (hex 20).
	__declspec( property( get = GetDateTaken, put = SetDateTaken ) )
		CString DateTaken;

	// public methods
public:
	// return the month of the year (1..12) given the month's name
	// or return 0 if one is not found
	int GetMonthOfTheYear( CString month )
	{
		int value = 0;

		// the key is the first three characters in lower case
		const CString csKey = month.Left( 3 ).MakeLower();

		// if the key exists in the cross reference, then lookup the
		// month of the year (1..12)
		if ( m_MonthLookup.Exists[ csKey ] )
		{
			value = *m_MonthLookup.find( csKey );
		}

		return value;
	}

	// constructor
	CDate()
	{
		Year = -1;
		Month = -1;
		Day = -1;
		Hour = 0;
		Minute = 0;
		Second = 0;

		// create a lookup table for months
		LPCTSTR months[] =
		{
			_T( "jan" ),
			_T( "feb" ),
			_T( "mar" ),
			_T( "apr" ),
			_T( "may" ),
			_T( "jun" ),
			_T( "jul" ),
			_T( "aug" ),
			_T( "sep" ),
			_T( "oct" ),
			_T( "nov" ),
			_T( "dec" ),
		};
		const int nMonths = _countof( months );
		for ( int nMonth = 0; nMonth < nMonths; nMonth++ )
		{
			m_MonthLookup.add
			(
				months[ nMonth ],
				new int( nMonth + 1 )
			);
		}

		Okay = false;
	}
};

////////////////////////////////////////////////////////////////////////////
// used for Gdiplus library
ULONG_PTR m_gdiplusToken;

/////////////////////////////////////////////////////////////////////////////
// this class records the date and time information in each image file 
// referenced
CDate m_Date;

////////////////////////////////////////////////////////////////////////////
// the number of hours the date taken metadata will be offset
double m_dHourOffset;

/////////////////////////////////////////////////////////////////////////////
// initialize GDI+
bool InitGdiplus()
{
	GdiplusStartupInput gdiplusStartupInput;
	Status status = GdiplusStartup
	(
		&m_gdiplusToken,
		&gdiplusStartupInput,
		NULL
	);
	return ( Ok == status );
} // InitGdiplus

/////////////////////////////////////////////////////////////////////////////
// remove reference to GDI+
void TerminateGdiplus()
{
	GdiplusShutdown( m_gdiplusToken );
	m_gdiplusToken = NULL;

}// TerminateGdiplus

/////////////////////////////////////////////////////////////////////////////
