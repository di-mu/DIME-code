#!/bin/bash

for file in *.txt; do
	grep -w "/ 2....." $file | head -1 >> duty-1.csv
	grep -w "/ 4....." $file | head -1 >> duty-2.csv
	grep -w "/ 7....." $file | head -1 >> duty-3.csv
	grep -w "/ 9....." $file | head -1 >> duty-4.csv
	grep -w "/ 12....." $file | head -1 >> duty-5.csv
	grep -w "/ 14....." $file | head -1 >> duty-6.csv
	grep -w "/ 16....." $file | head -1 >> duty-7.csv
	grep -w "/ 19....." $file | head -1 >> duty-8.csv
	grep -w "/ 21....." $file | head -1 >> duty-9.csv
	grep -w "/ 24....." $file | head -1 >> duty-10.csv
	grep -w "/ 26....." $file | head -1 >> duty-11.csv
	grep -w "/ 28....." $file | head -1 >> duty-12.csv
	grep -w "/ 31....." $file | head -1 >> duty-13.csv
	grep -w "/ 33....." $file | head -1 >> duty-14.csv
	grep -w "/ 36....." $file | head -1 >> duty-15.csv
	grep -w "/ 38....." $file | head -1 >> duty-16.csv
	grep -w "/ 40....." $file | head -1 >> duty-17.csv
	grep -w "/ 43....." $file | head -1 >> duty-18.csv
	grep -w "/ 45....." $file | head -1 >> duty-19.csv
	grep -w "/ 48....." $file | head -1 >> duty-20.csv
	grep -w "/ 50....." $file | head -1 >> duty-21.csv
	grep -w "/ 52....." $file | head -1 >> duty-22.csv
	grep -w "/ 55....." $file | head -1 >> duty-23.csv
	grep -w "/ 57....." $file | head -1 >> duty-24.csv
done