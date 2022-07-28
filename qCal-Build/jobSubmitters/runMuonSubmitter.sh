### Run Jobs for qCal
### Contact: james-wetzel@uiowa.edu
###
### $1 = number of events
### $2 = x axis
### $3 = y axis
### $4 = z axis

for E in 1 2 3 4 5 6 7 8 9 10 15 20 25 30 40 50 60 70 80 90 100 150 200 250 300 350 400 450 500 750 1000
do

	qsub -V -j y -q INFORMATICS muonSubmitter.job muons $E /Users/ploranger/qCal-rewritten10.7/qCal-Build $1 $2 $3 $4

done