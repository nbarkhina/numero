
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ezdib.h"

int bar_graph( HEZDIMAGE x_hDib, HEZDFONT x_hFont, int x1, int y1, int x2, int y2,
			   int nDataType, void *pData, int nDataSize, int *pCols, int nCols )
{
	int i, c, w, h;
	int tyw = 0, bw = 0;
	double v, dMin, dMax, dRMin, dRMax;

	// Sanity checks
	if ( !pData || 0 >= nDataSize || !pCols || !nCols )
		return 0;

	// Get the range of the data set
	ezd_calc_range( nDataType, pData, nDataSize, &dMin, &dMax, 0 );

	// Add margin to range
	dRMin = dMin - ( dMax - dMin ) / 10;
	dRMax = dMax + ( dMax - dMin ) / 10;

	if ( x_hFont )
	{	
		char num[ 256 ] = { 0 };
		
		// Calculate text width of smallest value
		sprintf( num, "%.2f", dMin );
		ezd_text_size( x_hFont, num, -1, &tyw, &h );
		ezd_text( x_hDib, x_hFont, num, -1, x1, y2 - ( h * 2 ), *pCols );

		// Calculate text width of largest value
		sprintf( num, "%.2f", dMax );
		ezd_text_size( x_hFont, num, -1, &w, &h );
		ezd_text( x_hDib, x_hFont, num, -1, x1, y1 + h, *pCols );
		if ( w > tyw )
			tyw	= w;
			
		// Text width margin
		tyw += 10;
	
	} // end if

	// Draw margins
	ezd_line( x_hDib, x1 + tyw - 2, y1, x1 + tyw - 2, y2, *pCols );
	ezd_line( x_hDib, x1 + tyw - 2, y2, x2, y2, *pCols );

	// Calculate bar width
	bw = ( x2 - x1 - tyw - nDataSize * 2 ) / nDataSize;

	// Draw the bars
	c = 0;
	for ( i = 0; i < nDataSize; i++ )
	{
		if ( ++c >= nCols )
			c = 1;

		// Get the value for this element
		v = ezd_scale_value( i, nDataType, pData, dRMin, dRMax - dRMin, 0, y2 - y1 - 2 );

		// Fill in the bar
		ezd_fill_rect( x_hDib, x1 + tyw + i + ( ( bw + 1 ) * i ), y2 - (int)v - 2,
							   x1 + tyw + i + ( ( bw + 1 ) * i ) + bw, y2 - 2, pCols[ c ] );

		// Outline the bar
		ezd_rect( x_hDib, x1 + tyw + i + ( ( bw + 1 ) * i ), y2 - (int)v - 2,
						  x1 + tyw + i + ( ( bw + 1 ) * i ) + bw, y2 - 2, *pCols );
	} // end for

	return 1;
}

#define PI		( (double)3.141592654 )
#define PI2		( (double)2 * PI )

int pie_graph( HEZDIMAGE x_hDib, int x, int y, int rad,
			   int nDataType, void *pData, int nDataSize, int *pCols, int nCols )
{
	int i, c;
	double v, pos, dMin, dMax, dTotal;

	// Sanity checks
	if ( !pData || 0 >= nDataSize || !pCols || !nCols )
		return 0;

	// Draw chart outline
	ezd_circle( x_hDib, x, y, rad, *pCols );

	// Get the range of the data set
	ezd_calc_range( nDataType, pData, nDataSize, &dMin, &dMax, &dTotal );

	// Draw the pie slices
	pos = 0; c = 0;
	ezd_line( x_hDib, x, y, x + rad, y, *pCols );
	for ( i = 0; i < nDataSize; i++ )
	{
		if ( ++c >= nCols )
			c = 1;

		// Get the value for this element
		v = ezd_scale_value( i, nDataType, pData, 0, dTotal, 0, PI2 );

		ezd_line( x_hDib, x, y,
						  x + (int)( (double)rad * cos( pos + v ) ),
						  y + (int)( (double)rad * sin( pos + v ) ),
						  *pCols );

		ezd_flood_fill( x_hDib, x + (int)( (double)rad / (double)2 * cos( pos + v / 2 ) ),
								y + (int)( (double)rad / (double)2 * sin( pos + v / 2 ) ),
								*pCols, pCols[ c ] );

		pos += v;

	} // end for

	return 1;
}

typedef struct _SAsciiData
{
	int sw;
	unsigned char *buf;
} SAsciiData;

int ascii_writer( void *pUser, int x, int y, int c, int f )
{
	SAsciiData *p = (SAsciiData*)pUser;
	unsigned char ch = (unsigned char)( f & 0xff );

	if ( !p )
		return 0;

	if ( ( '0' <= ch && '9' >= ch )
		 || ( 'A' <= ch && 'Z' >= ch )
		 || ( 'a' <= ch && 'z' >= ch ) )
		
		// Write the character
		p->buf[ y * p->sw + x ] = (unsigned char)f;

	else
		
		// Write the color
		p->buf[ y * p->sw + x ] = (unsigned char)c;
	
	return 1;
}

typedef struct _SDotMatrixData
{
	int w;
	int h;
	HEZDIMAGE pDib;
} SDotMatrixData;

int dotmatrix_writer( void *pUser, int x, int y, int c, int f )
{
	int cc, r, dw = 3;
	HEZDIMAGE hDib = (HEZDIMAGE)pUser;

	if ( !hDib )
		return 0;

	cc = c & 0xff;
	for ( r = 0; r < dw; r++ )
	{	ezd_circle( hDib, x * dw * 2 , y * dw * 2, r, cc );
		if ( r ) cc >>= 1;
	} // end for

	cc = ( c >> 8 ) & 0xff;
	for ( r = 0; r < dw; r++ )
	{	ezd_circle( hDib, x * dw * 2 + dw, y * dw * 2, r, cc << 8 );
		if ( r ) cc >>= 1;
	} // end for
		
	cc = c & 0xff;
	for ( r = 0; r < dw; r++ )
	{	ezd_circle( hDib, x * dw * 2 + dw, y * dw * 2 + dw, r, cc );
		if ( r ) cc >>= 1;
	} // end for

	cc = ( c >> 16 ) & 0xff;	
	for ( r = 0; r < dw; r++ )
	{	ezd_circle( hDib, x * dw * 2, y * dw * 2 + dw, r, cc << 16 );
		if ( r ) cc >>= 1;
	} // end for

	return 1;
}

int main( int argc, char* argv[] )
{
	int b, x, y;
	HEZDIMAGE hDib;
	HEZDFONT hFont;
	int bpp[] = { 1, 24, 32, 0 };

	//--------------------------------------------------------------
	// *** Normal example
	//--------------------------------------------------------------

	// For each supported pixel depth
	for ( b = 0; bpp[ b ]; b++ )
	{
		// Create output file name
		char fname[ 256 ] = { 0 };
		sprintf( fname, "test-%d.bmp", bpp[ b ] );
		printf( "Creating %s\n", fname );

		// Create image
		hDib = ezd_create( 640, -480, bpp[ b ], 0 );
		if ( !hDib )
			continue;

		// Set color threshold for mono chrome images
		if ( 1 == bpp[ b ] )
		{	ezd_set_color_threshold( hDib, 0x80 );
			ezd_set_palette_color( hDib, 0, 0x806000 );
			ezd_set_palette_color( hDib, 1, 0x000000 );
		} // end if

		// Fill in the background
		ezd_fill( hDib, 0x404040 );

		// Test fonts
		hFont = ezd_load_font( EZD_FONT_TYPE_MEDIUM, 0, 0 );
		if ( hFont )
			ezd_text( hDib, hFont, "--- EZDIB Test ---", -1, 10, 10, 0xffffff );

		// Draw random lines
		for ( x = 20; x < 300; x += 10 )
			ezd_line( hDib, x, ( x & 1 ) ? 50 : 100, x + 10, !( x & 1 ) ? 50 : 100, 0x00ff00 ),
			ezd_line( hDib, x + 10, ( x & 1 ) ? 50 : 100, x, !( x & 1 ) ? 50 : 100, 0x0000ff );

		// Random red box
		ezd_fill_rect( hDib, 200, 150, 400, 250, 0x900000 );

		// Random yellow box
		ezd_fill_rect( hDib, 300, 200, 350, 280, 0xffff00 );

		// Dark outline for yellow box
		ezd_rect( hDib, 300, 200, 350, 280, 0x000000 );

		// Draw random dots
		for ( y = 150; y < 250; y += 4 )
			for ( x = 50; x < 150; x += 4 )
				ezd_set_pixel( hDib, x, y, 0xffffff );

		// Circles
		for ( x = 0; x < 40; x++ )
			ezd_circle( hDib, 400, 60, x, x * 5 );

		// Draw graphs
		{
			// Graph data
			int data[] = { 11, 54, 23, 87, 34, 54, 75, 44, 66 };

			// Graph colors
			int cols[] = { 0xffffff, 0x400000, 0x006000, 0x000080 };

			// Draw bar graph
			ezd_rect( hDib, 35, 295, 605, 445, cols[ 0 ] );
			bar_graph( hDib, hFont, 40, 300, 600, 440, EZD_TYPE_INT,
					   data, sizeof( data ) / sizeof( data[ 0 ] ),
					   cols, sizeof( cols ) / sizeof( cols[ 0 ] ) );

			// Draw pie graph
			ezd_circle( hDib, 525, 150, 84, cols[ 0 ] );
			pie_graph( hDib, 525, 150, 80, EZD_TYPE_INT,
					   data, sizeof( data ) / sizeof( data[ 0 ] ),
					   cols, sizeof( cols ) / sizeof( cols[ 0 ] ) );

		}

		// Save the test image
		ezd_save( hDib, fname );

		/// Releases the specified font
		if ( hFont )
			ezd_destroy_font( hFont );

		// Free resources
		if ( hDib )
			ezd_destroy( hDib );

	} // end for

	//--------------------------------------------------------------
	// *** Example with user supplied static buffers
	//--------------------------------------------------------------

	// For each supported pixel depth
	for ( b = 0; bpp[ b ]; b++ )
	{
		const int w = 320, h = 240;
		const int r = ( ( w > h ) ? ( h >> 1 ) : ( w >> 1 ) ) - 10;
	
		// User buffer
		char user_header[ EZD_HEADER_SIZE ];
		char user_buffer[ 320 * 240 * 4 ];

		// Create output file name
		char fname[ 256 ] = { 0 };
		sprintf( fname, "user-%d.bmp", bpp[ b ] );
		printf( "Creating %s\n", fname );

		// Create image
		hDib = ezd_initialize( user_header, sizeof( user_header ), w, -h, bpp[ b ], EZD_FLAG_USER_IMAGE_BUFFER );
		if ( !hDib )
			continue;

		// Set user buffer
		if ( !ezd_set_image_buffer( hDib, user_buffer, sizeof( user_buffer ) ) )
			continue;

		// Fill in the background
		ezd_fill( hDib, 0x000000 );

		// Draw circles
		for ( x = 0; x < r; x += 4 )
			ezd_circle( hDib, w >> 1, h >> 1, x, x * 5 );

		// Save the test image
		ezd_save( hDib, fname );

		// Free resources
		if ( hDib )
			ezd_destroy( hDib );

	} // end for

	//--------------------------------------------------------------
	// *** Example using un-buffered user callback and no allocations
	//--------------------------------------------------------------

	// Pixel depth doesn't mean anything here
	// for ( b = 0; bpp[ b ]; b++ )
	{
		HEZDIMAGE hDmd;
		const int w = 640, h = 480;
		
		printf( "Creating dotmatrix.bmp\n" );
		
		// Create a 'fake' dot matrix display
		hDmd = ezd_create( w, -h, 24, 0 );
		if ( !hDmd )
			return -1;
		
		// Give our dot matrix display a black background
		ezd_fill( hDmd, 0 );

		// Create video 'driver'
		hDib = ezd_create( w, -h, 1, EZD_FLAG_USER_IMAGE_BUFFER );
		if ( !hDib )
			return -1;

		// Set pixel callback function
		ezd_set_pixel_callback( hDib, &dotmatrix_writer, hDmd );

		// Draw some text
		hFont = ezd_load_font( EZD_FONT_TYPE_MEDIUM, 0, 0 );
		if ( hFont )
			ezd_text( hDib, hFont, "Hello World!", -1, 4, 4, 0xa0a0a0 );

		{
			// Graph data
			int data[] = { 11, 54, 23, 87, 34, 54, 75, 44, 66 };

			// Graph colors
			int cols[] = { 0xffffff, 0x400000, 0x006000, 0x000080 };

			// Draw bar graph
			bar_graph( hDib, 0, 2, 16, 100, 70, EZD_TYPE_INT,
					   data, sizeof( data ) / sizeof( data[ 0 ] ),
					   cols, sizeof( cols ) / sizeof( cols[ 0 ] ) );

		}

		if ( hFont )
			ezd_destroy_font( hFont );

		if ( hDib )
			ezd_destroy( hDib );
			
		// Save the 'dotmatrix' image
		ezd_save( hDmd, "dotmatrix.bmp" );

		if ( hDmd )
			ezd_destroy( hDmd );
			
	} // end for

	//--------------------------------------------------------------
	// *** Example using ASCII buffer and no allocations
	//--------------------------------------------------------------

	// Pixel depth doesn't mean anything here
	// for ( b = 0; bpp[ b ]; b++ )
	{
		SAsciiData ad;
		const int w = 44, h = 20;
		char ascii[ ( 44 + 1 ) * 20 + 1 ];
		char user_header[ EZD_HEADER_SIZE ];
		
		hDib = ezd_initialize( user_header, sizeof( user_header ), w, -h, 1, EZD_FLAG_USER_IMAGE_BUFFER );
		if ( !hDib )
			return -1;
			
		// Null terminate
		ascii[ ( w + 1 ) * h ] = 0;
		
		// Fill in new lines
		for ( y = 0; y < h - 1; y++ )
			ascii[ y * ( w + 1 ) + w ] = '\n';
		
		// Set pixel callback function
		ad.sw = w + 1; ad.buf = ascii;
		ezd_set_pixel_callback( hDib, &ascii_writer, &ad );

		// Fill background with spaces
		ezd_fill( hDib, ' ' );
		
		// Border
		ezd_rect( hDib, 0, 0, w - 1, h - 1, '.' );
		
		// Head
		ezd_circle( hDib, 30, 10, 8, 'o' );

		// Mouth
		ezd_arc( hDib, 30, 10, 5, 0.6, 2.8, '-' );
		
		// Eyes
		ezd_set_pixel( hDib, 28, 8, 'O' );
		ezd_set_pixel( hDib, 32, 8, 'O' );
		
		// Nose
		ezd_line( hDib, 30, 10, 30, 11, '|' );
		
		// Draw some text
		hFont = ezd_load_font( EZD_FONT_TYPE_SMALL, 0, 0 );
		if ( hFont )
			ezd_text( hDib, hFont, "The\nEnd", -1, 4, 4, '#' );
		
		if ( hFont )
			ezd_destroy_font( hFont );

		if ( hDib )
			ezd_destroy( hDib );

		// Show our buffer
		printf( "%s\n", ascii );
			
	} // end for
	
	return 0;
}
