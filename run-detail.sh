#!/bin/bash

export PATH="/home/ken/App/ns-3.29/source/ns-3.29:$PATH"
# export PATH="/home/hiroshima-lenovo/App/ns3/source/ns-3.29:$PATH"

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
distance_disturbers=30

for y in -65 -55 -45 -35 -25 -15 -5 5 15 25 35 45 55 65
do
  echo "---------------------------"
  for i in `seq 5`
  do
    for x in -65 -55 -45 -35 -25 -15 -5 5 15 25 35 45 55 65
    do
      ./waf --run "HiddenWLANs --SimulationTime=$sim_time --EnableOutput=false --EnableOutputFile=true --PositionX=$x --PositionY=$y --DistanceDisturbers=$distance_disturbers" &
    done
    wait
    echo "Done!"
  done
done
