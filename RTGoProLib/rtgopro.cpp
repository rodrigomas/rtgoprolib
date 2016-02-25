#include "rtgopro.h"
#include "engine.h"

#define RTG_VERSION "0.0.0.4"

const char* rtgGetVersion()
{
	return RTG_VERSION;
}

void rtgPushImage(void *source)
{
	Engine::getInstance().addImage(source);
}

int rtgGetResultWidth()
{
	return Engine::getInstance().getResultWidth();
}

int rtgGetResultHeight()
{
	return Engine::getInstance().getResultHeight();
}

int rtgInitialize(const char *filename, int grd_width, int grd_height, int im_width, int im_height, bool flipY, bool flipRGB, bool usePolar, RotationType rotate, bool closeCycle, ICallback response )
{
	Engine& engine = Engine::getInstance();	

	return (engine.initialize(std::string(filename), grd_width, grd_height, im_width, im_height, flipY, flipRGB, usePolar, rotate, closeCycle, response) == true) ? 0 : 1;
}

void rtgSetMapping(MappingType firstPlane, MappingType secondPlane, MappingType thirdPlane, MappingType fourthPlane)
{
	Engine& engine = Engine::getInstance();	

	engine.setMapping(firstPlane, secondPlane, thirdPlane, fourthPlane);
}

void rtgStartEngine()
{
	Engine::getInstance().startEngine();
}

void rtgStopEngine()
{
	Engine::getInstance().stopEngine();
}

void rtgSetMessageCallback(IMsgCallback mes)
{
	Engine::getInstance().setMessageCallback(mes);
}
