#!/bin/bash

# export PATH="/home/ken/App/ns-3.29/source/ns-3.29:$PATH"
export PATH="/home/hiroshima-lenovo/App/ns3/source/ns-3.29:$PATH"

waf

if [ $? -ne 0 ]; then
  echo "Compile error."
  exit 1
fi

echo "Compile done!"
echo "---------------------------"
echo "Begin simulation ///"

pwd

sim_time=3
distance_disturbers=50

for y in -100 -90 -80 -70 -60 -50 -40 -30 -20 -10 0 10 20 30 40 50 60 70 80 90 100
do
  echo "---------------------------"
  for i in `seq 5`
  do
    for x in -100 -90 -80 -70 -60 -50
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    for x in -50 -40 -30 -20 -10 0
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    for x in 10 20 30 40 50
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    for x in 60 70 80 90 100
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    echo "Done!"
  done
done
