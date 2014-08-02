#!/bin/bash

latestversion=008
latestdate=140802
latestDL=http://forum.xda-developers.com/showthread.php?t=2765196
latestdatestring=$(date -d "$latestdate" "+%A, %B %d %Y")

input="$1"
if [[ -z "$input" ]]
	then
	exit 1
fi

echo ${!input}
