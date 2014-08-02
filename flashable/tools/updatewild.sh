#!/bin/bash

latestversion=008
# date "+%y%m%d"
latestdate=140802
latestDL="http://forum.xda-developers.com/showthread.php?t=2765196"
#date "+%A, %B %d %Y"
latestdateliteral="Saturday, August 02 2014"

input="$1"
if [[ -z "$input" ]]
	then
	exit 1
fi

echo ${!input}
