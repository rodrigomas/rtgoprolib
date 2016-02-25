//! Realtime GoPro Stitcher GPU
/*!
  Realtime GoPro Stitcher GPU Library Header

  \author Rodrigo Marques

  \version 0.0.0.1 
   - Initial Version - (30/01/2016) - Serial Load, Process and Download. Parallel buffer fetch.

  \version 0.0.0.2 
   - Add Rotation Support
   - Add Cycle Support

  \version 0.0.0.3
   - Add Horizontal Bend Support
   - Framebuffer dimension recompute

  \version 0.0.0.4
   - Add Mapping Support
*/
#ifndef _RTGOPRO_
#define _RTGOPRO_

#ifdef DLLEXPORT_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

extern "C"
{
	/**
	 * \enum RotationType 
	 * \brief specifies the rotation type
	 */

	typedef enum 
	{
		rtNone = 0, /**< No rotation */
		rt90Degrees, /**< 90 Degrees rotation */
		rtMinus90Degrees  /**< -90 Degrees rotation */

	} RotationType;

	/**
	 * \enum MappingType 
	 * \brief specifies the quadrant type
	 */

	typedef enum 
	{
		mtTopLeft = 0, /**< Top Left*/
		mtTopRight = 1, /**< Top Right */
		mtBottomLeft = 2, /**< Bottom Left */
		mtBottomRight = 3, /**< Bottom Right */

	} MappingType;

	/**
	 \brief Response Callback
	 \param output Stitch Output (RGBA unsigned byte)
	 \param input Input Image Used
	*/
	typedef void (*ICallback)(void*output, void*input);

	/**
	 \brief Message Callback

	 Callback used to send error and information to caller application

	 \param message Error or information message
	 
	*/
	typedef void (*IMsgCallback)(const char*message);


	/**
	 \brief Push and Image to the Internal Queue

	 \param source Source Image (RGB unsigned byte), width and height was setted at initialization process
	 
	 \waring You cannot release the memory until receive the response callback related by the source.

	*/
	DLLEXPORT void rtgPushImage(void *source);	

	/**
	 \brief Get the Width of the Stitch output image

	 \return Return the Width of the Stitch output image

	*/
	DLLEXPORT int rtgGetResultWidth();

	/**
	 \brief Get the library version

	 \return Return the library version

	*/
	DLLEXPORT const char* rtgGetVersion();

	/**
	 \brief Get the Height of the Stitch output image

	 \return Return the Height of the Stitch output image

	*/
	DLLEXPORT int rtgGetResultHeight();

	/**
	 \brief Get the Height of the Stitch output image

	 \param filename Stitch Configuration File (V02)
	 \param grd_width Specify Grid Width (quad mesh) (default: 64)
	 \param grd_height Specify Grid height (quad mesh) (default: 64)
	 \param im_width Specify Image Width (full image size)
	 \param im_height Specify Image Height (full image size)
	 \param flipY Image must be vertically flipped
	 \param flipRGB Image must flip the R and B components
	 \param usePolar Force use of polar distortion algorithm 
	 \param rotate Rotate the image by 90 degrees, -90 degrees or 0 degrees (no rotation)
	 \param closeCycle Force cycle (360)
	 \param response Callback to receive the result image

	 \return Return 1 if had a error or 0 otherwise

	 \warning The callback hold the processing algorithm, you must copy the resulting buffer as soon as possible.

	*/
	DLLEXPORT int rtgInitialize(const char *filename, int grd_width, int grd_height, int im_width, int im_height, bool flipY, bool flipRGB, bool usePolar, RotationType rotate, bool closeCycle, ICallback response );


	/**
	 \brief Set the Quadrant Mapping for the Input. It can be used on the fly.

	 \param firstPlane First Plane to Map (default: TopLeft)
	 \param secondPlane Second Plane to Map (default: TopRight)
	 \param thirdPlane Third Plane to Map (default: BottomLeft)
	 \param fourthPlane Fourth Plane to Map (default: BottomRight)

	 \warning Do not use other values, only MappingType values.

	*/
	DLLEXPORT void rtgSetMapping(MappingType firstPlane, MappingType secondPlane, MappingType thirdPlane, MappingType fourthPlane);	

	/**
	 \brief Start the stitch processor engine
	*/
	DLLEXPORT void rtgStartEngine();

	/**
	 \brief Stop the stitch processor engine
	*/
	DLLEXPORT void rtgStopEngine();	

	/**
	 \brief Set the internal message callback to receive information and error
	*/
	DLLEXPORT void rtgSetMessageCallback(IMsgCallback mes);
}

#endif