#!/bin/bash

for file in *.txt; do
	grep -w "/ 240..." $file | head -1 >> duty-1.csv
	grep -w "/ 480..." $file | head -1 >> duty-2.csv
	grep -w "/ 720..." $file | head -1 >> duty-3.csv
	grep -w "/ 960..." $file | head -1 >> duty-4.csv
	grep -w "/ 1200..." $file | head -1 >> duty-5.csv
	grep -w "/ 1440..." $file | head -1 >> duty-6.csv
	grep -w "/ 1680..." $file | head -1 >> duty-7.csv
	grep -w "/ 1920..." $file | head -1 >> duty-8.csv
	grep -w "/ 2160..." $file | head -1 >> duty-9.csv
	grep -w "/ 2400..." $file | head -1 >> duty-10.csv
	grep -w "/ 2640..." $file | head -1 >> duty-11.csv
	grep -w "/ 2880..." $file | head -1 >> duty-12.csv
	grep -w "/ 3120..." $file | head -1 >> duty-13.csv
	grep -w "/ 3360..." $file | head -1 >> duty-14.csv
	grep -w "/ 3600..." $file | head -1 >> duty-15.csv
	grep -w "/ 3840..." $file | head -1 >> duty-16.csv
	grep -w "/ 4080..." $file | head -1 >> duty-17.csv
	grep -w "/ 4320..." $file | head -1 >> duty-18.csv
	grep -w "/ 4560..." $file | head -1 >> duty-19.csv
	grep -w "/ 4800..." $file | head -1 >> duty-20.csv
	grep -w "/ 5040..." $file | head -1 >> duty-21.csv
	grep -w "/ 5280..." $file | head -1 >> duty-22.csv
	grep -w "/ 5520..." $file | head -1 >> duty-23.csv
	grep -w "/ 5760..." $file | head -1 >> duty-24.csv
done