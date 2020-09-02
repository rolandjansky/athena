/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMDatabaseID

    This class represents an unique ID which is used to identify Performance Parameters
    in a database. Seven Identifier Tags are used to create the ID. The ID itself is
    represented as String. See ATLAS-Note "" for detailed information 

    @author  Jovan.Mitrevski (this version), Matthias.Schott@cern.ch (original APDatabaseID)
**/
/////////////////////////////////////////////////////////////////////////////


#ifndef EMDATABASEID_H
#define EMDATABASEID_H

#include <string>
#include <utility>

struct EMDatabaseIDDescriptor
{
  std::string	Object;		/** Object: Muon, IDTrack, Electron, Tau, Jet, ... */
  std::string	Container;	/** Container: StacoMuonCollection, Trigger_MU20, ElectronAODCollection */
  std::string	Type;		/** Type: Efficiency, Scale/Resolution, Fake, ... */
  std::string	Channel;	/** Channel: Zmumu, Zee, JPsi, TTbarSemiLeptonic */
  std::string	Author;		/** Author: PerformanceGroup, PrivateMSchott, ....*/
  std::string	RecoSWV;	/** SoftwareVersionForReconstruction: 14.0.12, ...*/
  std::string	Tag;		/** Additonal User Information, ...; in egamma used for parametrization: "ClusterEta ClusterE" **/
  long		runStart;	/** Run Number, where the data analysis has started*/
  long		runEnd;		/** Run Number, where the data analysis has stopped*/
  std::string	SimSWV;		/** Software-Version which was used for simulation - eventually also AMI Tag*/
};


class EMDatabaseID
{
public:
  /** Default constructor */
  EMDatabaseID();
  /** Copy constructor */
  EMDatabaseID(const EMDatabaseID& ob);
  /** Constructor via unique id-string*/
  EMDatabaseID(const std::string& id);
  /** Constructor via unique id-string*/
  EMDatabaseID(EMDatabaseIDDescriptor &id);
  /** Constructor via 8 Identifiers for performance parameters which have been determined in a specific run*/
  EMDatabaseID(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, long start, long end);
  /** Constructor via 7 Identifiers for performance parameters which have been determined in Monte Carlo Simulation*/
  EMDatabaseID(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, const std::string& SimSWV);
  
  // this constructor is used for object retrieval; it only contains the subset that is needed
  EMDatabaseID(const std::string& Object, const std::string& Type, const std::string& Tag);

  /** Destructor*/
  ~EMDatabaseID();

  /** = operator*/
  EMDatabaseID& operator = (const EMDatabaseID& ob);
  /** == operator: returns true if all Identifiers of the two objects coincide.*/
  bool operator == (const EMDatabaseID& ob) const;
  /** Returns true if the given Identifiers in the parameter ob agree with the given object. 
      Identifiers of "ob" which have been initialized empty, i.e. "" are not considered.*/
  bool isInSubgroup(const EMDatabaseID& ob) const;
  /** Set given ID to this object*/
  void set(const EMDatabaseID& ob);
  /** Set ID of this object via 8 Identifiers which describe a specific data-run*/
  void set(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, long start, long end);
  /** Set ID of this object via 7 Identifiers which describe a Monte Carlo sample*/
  void set(const std::string& Object, const std::string& Container, const std::string& Type, const std::string& Channel, const std::string& Author, const std::string& RecoSWV, const std::string& Tag, const std::string& SimSWV);

  // this constructor is used for object retrieval; it only contains the subset that is needed
 void set(const std::string& Object, const std::string& Type, const std::string& Tag);

  /** Clear all ID-informations*/
  void clear();

  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void object(std::string s) 		{	m_idDes.Object = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void container(std::string s) 	{	m_idDes.Container = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void type(std::string s) 		{	m_idDes.Type = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void channel(std::string s) 	{	m_idDes.Channel = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void author(std::string s) 		{	m_idDes.Author = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void recoSWV(std::string s) 	{	m_idDes.RecoSWV = beautify(std::move(s));}
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally*/
  void tag(std::string s) 	{	m_idDes.Tag = beautify(std::move(s));}
  /** Access to single identifier. If you set start and end-run numbers you also choose 
      that you want to store run-data*/
  void run(long start, long end) 	{	m_idDes.runStart = start; m_idDes.runEnd = end; m_idDes.SimSWV=""; }
  /** Access to single identifier. Note, that characters '_',' ','/' and '\\' are deleted internally. If you choose the Software-Version which is used for 
      simulation you also choose that you want to store MC-related data*/
  void simSWV(std::string s) 		{	m_idDes.SimSWV = beautify(std::move(s)); m_idDes.runStart = 0; m_idDes.runEnd = 0;}

  /** converts the given string to runNumbers or Softwareversion. return 0 in case of failure. returns 1 in case of run and 2 inclase of MCDataset */
  int setRunOrMCSWV(std::string s);

  /** Access to single identifier*/
  std::string object() const	{	return m_idDes.Object;}
  /** Access to single identifier*/
  std::string container() const {	return m_idDes.Container;}
  /** Access to single identifier*/
  std::string type() const		{	return m_idDes.Type;}
  /** Access to single identifier*/
  std::string channel() const	{	return m_idDes.Channel;}
  /** Access to single identifier*/
  std::string author() const	{	return m_idDes.Author;}
  /** Access to single identifier*/
  std::string recoSWV() const	{	return m_idDes.RecoSWV;}
  /** Access to single identifier*/
  std::string tag() const	{	return m_idDes.Tag;}
  /** Access to single identifier*/
  long runStart() const 		{	return m_idDes.runStart; }
  /** Access to single identifier*/
  long runEnd() const			{	return m_idDes.runEnd; }
  /** Access to single identifier*/
  std::string simSWV() 	{	return m_idDes.SimSWV; }

  /** Returns true, if this is an identifier for a MC-determined Performance Quantaty. Otherwise false*/
  bool isMCData() const;
  /** Return true, if all 7/8 Identifiers have been initialized*/
  bool isComplete() const;
  /** Returns the unique ID-String of this Object*/
  std::string getUniqueID() const;
  /** Set the unique ID-String and initialize the object. Returns true if everything went fine, i.e. all
      parameters/formatation was ok*/
  bool setUniqueID(const std::string& s); // note that this is an "init"-like operation

  // the functions below are to simplify the storage and retrieval

  // these must use only the variables set in the read-out version of the constructor
  // get the ROOT (not COOL) directory name
  std::string getRootDirectory() const { return object(); }; 
  std::string getCoolChannel() const; // get the COOL channel name
  std::string getTreeName() const { return type(); };
  std::string getBranchName() const { return tag(); };

  EMDatabaseIDDescriptor getIDDescriptor() const {return m_idDes;}

private:
  /** deletes characters '_',' ','/' and '\\' out of the string*/
  std::string beautify(const std::string& s) const;
  /** converts int to string */
  std::string intToString(long l) const;

  EMDatabaseIDDescriptor	m_idDes;	/** Descriptor of DatabaseID*/
};

#endif
