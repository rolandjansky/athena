/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthController_H
#define TruthController_H

class TrackVisualizationHelper;

class TruthController {
private:
	// this is a singleton
	static TruthController *s_thePointer;
	TruthController();
	TruthController(const TruthController&) {}
        TruthController& operator= (const TruthController&); 
	~TruthController();
	TrackVisualizationHelper *m_theVisHelper;
public:
	static TruthController *GetTruthController();
	void SetVisualizationHelper(TrackVisualizationHelper *);
	TrackVisualizationHelper* GetVisualizationHelper() const 
	{
		return m_theVisHelper;
	}
};

#endif
