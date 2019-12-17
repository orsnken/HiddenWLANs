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

for y in -65 -60 -55 -50 -45 -40 -35 -30 -25 -20 -15 -10 -5 0 5 10 15 20 25 30 35 40 45 50 55 60 65
do
  echo "---------------------------"
  for i in `seq 1`
  do
    for x in -65 -55 -45 -35 -25 -15 -5 5 15 25 35 45 55 65
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    for x in -60 -50 -40 -30 -20 -10 -10 0 10 20 30 40 50 60
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    echo "Done!"
  done
done
