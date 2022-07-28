### Run Jobs for qCal
### Contact: james-wetzel@uiowa.edu
###
### $1 = number of events
### $2 = x axis
### $3 = y axis
### $4 = z axis

for E in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 1.5 2.0 2.5 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 15.0 20.0 25.0 30.0 35.0 40.0 45.0 50.0 75.0 100.0
do

	qsub -V -j y -q INFORMATICS electronSubmitter.job electrons $E /Users/ploranger/qCal-rewritten10.7/qCal-Build $1 $2 $3 $4

done