#!/bin/sh

latestversion=008
# date "+%y%m%d"
latestdate=140802
latestDL="http://forum.xda-developers.com/showthread.php?t=2765196"
#date "+%A, %B %d %y"
latestdateliteral="Saturday, August 02 2014"

input="$1"
if [[ -z "$input" ]]
	then
	exit 1
fi

if [[ "$input" == "latestversion" ]]
	then
	echo $latestversion

elif [[ "$input" == "latestdate" ]]
	then
	echo $latestdate

elif [[ "$input" == "latestDL" ]]
	then
	echo $latestDL
elif [[ "$input" == "latestdateliteral" ]]
	then
	echo $latestdateliteral
fi

# echo ${!input} Apparently not all roms have bash installed ._.
