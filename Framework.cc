#include "ns3.h"

#include "Framework.h"

#include <random>

Framework::Framework() {
  std::random_device rnd;
  std::mt19937 mt(rnd());
  std::uniform_int_distribution<> rand1000(0, 1000);
  SetSeed(rand1000(mt) + 1);
  SetRunNumber(rand1000(mt) + 1);
}

Framework& Framework::instance() {
  static Framework sInstance;
  return sInstance;
}

void Framework::SetSeed(int seed) {
  seed_ = (seed > 0)? seed: seed_;
  ns3::RngSeedManager::SetSeed(seed_);
}

void Framework::SetRunNumber(int run_number) {
  run_number_ = (run_number > 0)? run_number: run_number_;
  ns3::RngSeedManager::SetRun(run_number_);
}

int Framework::GetSeed() const {
  return seed_;
}

int Framework::GetRunNumber() const {
  return run_number_;
}
