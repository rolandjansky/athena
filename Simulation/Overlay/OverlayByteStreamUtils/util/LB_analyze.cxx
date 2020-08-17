/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <TRandom3.h>


struct lbn_info
{
  int nevt{};
  float intlumi{};
  int nevtwanted{};
};

struct run_info
{
  int nevt{};
  float intlumi{};
  std::map<int, lbn_info> lbnmap;
  int nevtwanted{};
};


int readeventcounts(int run,
                    std::map<int, int> &eventcounts)
{
  eventcounts.clear();
  char buf[50];
  sprintf(buf, "lbnevents_%d.txt", run);
  printf("Opening %s, eventcounts size is %lu\n", buf, eventcounts.size());
  FILE *fp = fopen(buf, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open %s!!!\n", buf);
    return 1;
  }

  char *line = new char[500];
  while (fgets(line, 480, fp)) {
    int lbn, ne, nf, runn;
    float mb;
    int s = sscanf(&line[0], " ... Run %d, LB %d has %d events, %d RAW files and %f MB", &runn, &lbn, &ne, &nf, &mb);
    if (s > 4) {
      if (run != runn) {
        return 20;
      }
      printf("run %d lbn %d has %d events\n", run, lbn, ne);
      eventcounts[lbn] = ne;
    } else {
      fprintf(stderr, "s=%d, bad read?\n", s);
      return 20;
    }
  }
  delete[] line;

  printf("Closing %s, eventcounts size is %lu\n", buf, eventcounts.size());
  fclose(fp);

  return 0;
}

int lbn_analyze(int stream,
                int nwanted,
                FILE *outfile)
{
  FILE *fp = fopen("lbn", "r");
    if (!fp) {
    fprintf(stderr, "Failed to open lbn input file!!!\n");
    return 1;
  }

  char *line = new char[500];
  int run, lbn, L1Acc, beforeps, afterps, valid;
  int liveL1, livebp, liveap;
  float instlumi, dt, avevtperbx, live, L1p, HLTp, LARp;
  int grun{};
  float gtotaltotallumi{}, gtotaltotallumiprescaled{};
  float gtotallumi{}, gtotallumiprescaled{};

  TRandom myrand;
  myrand.SetSeed(0);
  printf("- myrand seed is %u\n", myrand.GetSeed());
  myrand.SetSeed(myrand.Integer(10000000) + stream);
  printf("- myrand seed is now %u\n", myrand.GetSeed());

  std::map<int, run_info> runmap;
  std::map<int, int> eventcounts;
  while (fgets(line, 480, fp)) {
    if (line[0] != '-') {
      continue;
    }

    int s = sscanf(&line[0],
      "--- LumiCalculator      : %d[%d]: L1Acc: %d, Livetime trigger L1Acc: %d, InstLumi: %f, deltaT: %f, AvEvtsPerBX: %f, BeforePrescale: %d, AfterPrescale: %d, Livetime trigger BeforePrescale: %d Livetime trigger AfterPrescale: %d, Livefrac: %f, L1Presc: %f, HLTPresc: %f, Valid: %d, LAr ready fraction: %f",
      &run, &lbn, &L1Acc, &liveL1, &instlumi, &dt, &avevtperbx, &beforeps, &afterps, &livebp, &liveap, &live, &L1p, &HLTp, &valid, &LARp);

    if (s > 8) {
      printf("Got %d values : run=%d, lbn=%d, L1Acc=%d, instlumi=%f, L1p=%f, dt=%f, live=%f, afterps=%d \n", s, run, lbn, L1Acc, instlumi, L1p, dt, live, afterps);

      if (run != grun) {
	      if (grun > 0) { //change of run
	        runmap[grun].intlumi = gtotallumi;
          printf("Setting lumi for run %d to %f\n", grun, gtotallumi);
	        gtotaltotallumi += gtotallumi;
          gtotaltotallumiprescaled += gtotallumiprescaled;
          // reset the int lumi calculation for this run
	        gtotallumi = 0;
          gtotallumiprescaled = 0;
	      }

	      grun = run; //save the run number
	      printf("Setting grun to %d\n", run);

	      // read in event counts from runquery file for run
	      int status = readeventcounts(run, eventcounts);
        if (status != 0) {
          return status;
        }
      } //new run

      if (L1p < 0) {
	      runmap[run].lbnmap[lbn].nevt = 0;
	      if (eventcounts[lbn] !=0 ) {
          printf("For lbn %d, L1p<0 but eventcounts is %d\n",lbn,eventcounts[lbn]);
        }
      } else {
        int ne = L1Acc;//afterps
        if (ne != eventcounts[lbn]) {
          if (stream == 0) {
            printf("For lbn %d, ne from lumicalc is %d but we will trust runquery value of %d\n", lbn, ne, eventcounts[lbn]);
          }
          ne = eventcounts[lbn];
        }
        if (ne == 0) {
          dt = 0;
          printf("For lbn %d, setting lumi to 0 since 0 events were recorded\n", lbn);
        }
        runmap[run].lbnmap[lbn].nevt = ne;
        runmap[run].lbnmap[lbn].intlumi = instlumi * dt * live;
        runmap[run].nevt += ne;
        gtotallumiprescaled += instlumi * dt * live / L1p;
        gtotallumi += instlumi * dt * live;
       }
       printf("grun=%d, gtotallumi=%f, gtotallumiprescaled=%f\n", grun, gtotallumi, gtotallumiprescaled);
    } //good line
  } //loop over lines in file
  delete[] line;

  // after last run
  runmap[grun].intlumi = gtotallumi;
  printf("Setting lumi for run %d to %f\n", grun, gtotallumi);
  gtotaltotallumi += gtotallumi;
  gtotaltotallumiprescaled += gtotallumiprescaled;

  fclose(fp);

  printf("- %lu runs, gtotaltotallumi=%f, gtotaltotallumiprescaled=%f\n", runmap.size(), gtotaltotallumi, gtotaltotallumiprescaled);

  if (runmap.size() < 1) {
    printf("- runmap size is %lu, quitting!\n", runmap.size());
    return 0;
  }

  //check the total lumi...
  double tempr{};
  for (const auto &[r, info] : runmap) {
    tempr += info.intlumi;
  }
  if (fabs(tempr - gtotaltotallumi) / tempr > 0.001) {
    fprintf(stderr, "tempr=%f and gtotaltotallumi=%f\n", tempr, gtotaltotallumi);
    return 3;
  }

  for (int e = 0; e < nwanted; ++e) {
    // pick a random run, proportional to intlumi
    double rnd = myrand.Uniform(tempr);
    run = -1;
    for (const auto &[r, info] : runmap) {
      if (rnd < info.intlumi) {
        run = r;
        break;
      }
      rnd -= info.intlumi;
    }
    if (run < 0) { // error
      return 4;
    }

    // check the total run lumi...
    double tempt{};
    for (const auto &[l, info] : runmap[run].lbnmap) {
      tempt += info.intlumi;
    }
    if (fabs(tempt - runmap[run].intlumi) / tempt > 0.001) {
      fprintf(stderr, "tempt=%f and runmap[%d].intlumi=%f\n", tempt, run, runmap[run].intlumi);
      return 5;
    }

    //p ick a random lbn, proportional to intlumi
    rnd = myrand.Uniform(tempt);
    lbn = -1;
    for (const auto &[l, info] : runmap[run].lbnmap) {
      if (rnd < info.intlumi) {
        lbn = l;
        break;
      }
      rnd -= info.intlumi;
    }
    if (lbn < 0) { // error
      return 6;
    }

    runmap[run].nevtwanted++;
    runmap[run].lbnmap[lbn].nevtwanted++;
    printf("- stream %d, run %d, lbn %d, choose %d of out %d\n", stream, run, lbn, runmap[run].lbnmap[lbn].nevtwanted, runmap[run].lbnmap[lbn].nevt);
  } //loop over nwanted

  for (const auto &[r, rinfo] : runmap) {
    int totnevt{};
    float totintlumi{};
    printf("stream %d, run %d, has %d events and %f/ub, %f intlumi of total, and %d wanted\n", stream,
      r, rinfo.nevt, rinfo.intlumi, rinfo.intlumi / gtotaltotallumi, rinfo.nevtwanted);
    for (const auto &[l, linfo] : rinfo.lbnmap) {
      fprintf(outfile, "stream %d, run %d, lbn %d, has %d events and %f/ub, %f intlumi of run, and %d wanted", stream, r,
      l, linfo.nevt, linfo.intlumi, linfo.intlumi / rinfo.intlumi, linfo.nevtwanted);
      if (linfo.nevtwanted > linfo.nevt) {
        fprintf(outfile, " : WARNING, more than available, will be duplicates!\n");
      } else {
        fprintf(outfile,"\n");
      }
      totnevt += linfo.nevt;
      totintlumi += linfo.intlumi;
    }
    if (totnevt != rinfo.nevt) {
      fprintf(stderr, "  XXX events do not agree !!! \n");
    }
    if (totintlumi > 0.0 && fabs(totintlumi - rinfo.intlumi) / totintlumi > .001) {
      fprintf(stderr, "  XXX intlumi does not agree !!! %f %f \n", totintlumi, rinfo.intlumi);
    }
  }

  printf("--\n\n");

  return 0;
}

int main(int argc, const char* argv[])
{
  int Nstreams{50};   // number of zerobias streams to make
  int Nwanted{50000}; // number of events per zerobias stream

  if (argc == 3) {
    Nstreams = std::atoi(argv[1]);
    Nwanted = std::atoi(argv[2]);
  }

  printf("Nstreams=%d, Nwanted=%d\n\n", Nstreams, Nwanted);

  FILE *outfile = fopen("lbn_anal_map.txt", "w");

  for (int i = 0; i < Nstreams; ++i) {
    int status = lbn_analyze(i, Nwanted, outfile);
    if (status != 0) {
      return status;
    }
  }

  fclose(outfile);

  return 0;
}
