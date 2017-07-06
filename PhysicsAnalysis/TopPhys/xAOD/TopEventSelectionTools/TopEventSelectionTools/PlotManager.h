/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLOTMANAGER_H_
#define PLOTMANAGER_H_

#include <string>
#include <unordered_map>

class TFile;
class TH1;
class TDirectory;

namespace EL {
  class Worker;
}

namespace top {

/**
 * @brief A map that lets you create a bunch of histograms and get hold of them
 * via a string name.  It takes care of writing them to the outputfile too - if
 * you have one.
 */
class PlotManager {
public:
    /**
     * @brief A name for the selection.  Plots will be stored in a folder
     * with this name in the output file.  e.g. ejets, mujets etc.
     *
     * @param name The name of the folder to store stuff in the output file.
     * During the run name_ is added to the front of the histogram name, instead
     * of a folder because... well... ROOT!
     */
    explicit PlotManager(const std::string& name, TFile* outputFile = nullptr, EL::Worker* wk = nullptr);
    
    PlotManager(const std::string& sample,const std::string& channel,const std::string& syst,TFile* outputFile);

    /**
     * @brief Add a histogram to the output file.
     *
     * @param hname A unique name, notice that we automatically add the name
     * of the analysis on to this with an underscore.
     * @param title The histogram title, as root expects.
     * @param bins The number of bins, passed directly to root.
     * @param start The starting value, passed directly to root.
     * @param end The ending value, passed directly to root.
     */
    void addHist(const std::string& hname, const std::string& title, int bins, double start, double end) const;

    /**
     * @brief Add a 2D histogram to the output file.
     *
     * @param hname A unique name, notice that we automatically add the name
     * of the analysis on to this with an underscore.
     * @param title The histogram title, as root expects.
     * @param xbins The number of x-bins, passed directly to root.
     * @param xstart The starting x-value, passed directly to root.
     * @param xend The ending x-value, passed directly to root.
     * @param xbins The number of y-bins, passed directly to root.
     * @param xstart The starting y-value, passed directly to root.
     * @param xend The ending y-value, passed directly to root.
     */
    void addHist(const std::string& hname, const std::string& title,
		 int xbins, double xstart, double xend,
		 int ybins, double ystart, double yend) const;

    /**
     * @brief Recover an existing histogram, to fill it for example.
     *
     * @return A pointer to the histogram if it exists.  If it doesn't exist
     * then it'll exit, since that's clearly a bug.
     */
    TH1* hist(const std::string& name) const;

    /**
     * @brief Save the histograms in alphabetical order.
     *
     * First copy from the unordered map to an ordered one.  That helps
     * people when they look through the plots in a TBrowser.  Then call write
     * on each histogram.  I'm assuming you have an open file already.
     */
    void scaleHistograms(double sf = 1.) const;

private:
    EL::Worker* m_wk;

    ///Name of the folder to store the plots in, in the output file.
    std::string m_name;

    ///A map that's hopefully quick to search because it'll be used a lot per event.
    mutable std::unordered_map<std::string, TH1*> m_histograms;

    mutable TDirectory* m_plotDir;
};

}

#endif
