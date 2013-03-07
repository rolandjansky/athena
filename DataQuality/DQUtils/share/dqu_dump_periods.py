#! /usr/bin/env python

from DQUtils.periods import fetch_project_period_runs

def main():
    project_periods = fetch_project_period_runs()
    
    for project, period_runs in sorted(project_periods.iteritems()):
        for period, runs in sorted(period_runs.iteritems()):
            if period == "AllYear":
                filename = "{0}.AllYear.runs.list".format(project)
            else:
                filename = "{0}.period{1}.runs.list".format(project, period)
            with open(filename, "w") as fd:
                fd.write("\n".join(str(run) for run in runs) + "\n")

if __name__ == "__main__":
    main()
