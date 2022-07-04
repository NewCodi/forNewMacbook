#pragma once

void dummyMain(void);
void testCluster1(void);
void testEVChargerCluster(void);

namespace chip{
namespace Shell{

class DummyCommands
{
public:
	//delete copy constructor
	DummyCommands(const DummyCommands&) = delete;
	//delete move constructor
	DummyCommands(DummyCommands &&) = delete;
	//delete assignment operator
	DummyCommands& operator=(const DummyCommands&) = delete;

	static DummyCommands& Singleton()
	{
		static DummyCommands single;
		return single;
	}

	void Register();

private:
	DummyCommands() {}
};

}
}