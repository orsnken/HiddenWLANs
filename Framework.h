#ifndef INCLUDED_NS3_MURASE_LAB_FRAMEWORK_H__
#define INCLUDED_NS3_MURASE_LAB_FRAMEWORK_H__

class Framework {
private:
  Framework();
  ~Framework() = default;
public:
  Framework(const Framework&) = delete;
  Framework& operator=(const Framework&) = delete;
  Framework(Framework&&) = delete;
  Framework& operator=(Framework&&) = delete;

  static Framework& instance();

  void InitArguments(int argc, char *argv[]);
    // USER DEFINE
  void InitSimulation();
    // USER DEFINE
  void Run();
    // USER DEFINE

  void SetSeed(int seed);
  void SetRunNumber(int run_number);

  int GetSeed() const;
  int GetRunNumber() const;
private:
  int seed_;
  int run_number_;
};

#endif // INCLUDED_NS3_MURASE_LAB_FRAMEWORK_H__
