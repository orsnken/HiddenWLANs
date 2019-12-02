#include "ns3.h"

#include "Framework.h"

using namespace ns3;

int main(int argc, char *argv[]) {
  Framework& sim = Framework::instance();
  sim.InitArguments(argc, argv);
  sim.InitSimulation();
  sim.Run();
}
