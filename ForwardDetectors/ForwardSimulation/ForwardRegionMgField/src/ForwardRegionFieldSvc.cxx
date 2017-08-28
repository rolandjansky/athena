/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionFieldSvc.h"

#include <fstream>

#include "CLHEP/Geometry/Transform3D.h"

#include "GaudiKernel/IIncidentSvc.h"

#include "PathResolver/PathResolver.h"

// static ForwardRegionMgField q1("Q1",ForwardRegionField::Q1);
// static ForwardRegionMgField q2("Q2",ForwardRegionField::Q2);
// static ForwardRegionMgField q3("Q3",ForwardRegionField::Q3);
// static ForwardRegionMgField d1("D1",ForwardRegionField::D1);
// static ForwardRegionMgField d2("D2",ForwardRegionField::D2);
// static ForwardRegionMgField q4("Q4",ForwardRegionField::Q4);
// static ForwardRegionMgField q5("Q5",ForwardRegionField::Q5);
// static ForwardRegionMgField q6("Q6",ForwardRegionField::Q6);
// static ForwardRegionMgField q7("Q7",ForwardRegionField::Q7);
// static ForwardRegionMgField q1hkick("Q1HKick",ForwardRegionField::Q1HKick);
// static ForwardRegionMgField q1vkick("Q1VKick",ForwardRegionField::Q1VKick);
// static ForwardRegionMgField q2hkick("Q2HKick",ForwardRegionField::Q2HKick);
// static ForwardRegionMgField q2vkick("Q2VKick",ForwardRegionField::Q2VKick);
// static ForwardRegionMgField q3hkick("Q3HKick",ForwardRegionField::Q3HKick);
// static ForwardRegionMgField q3vkick("Q3VKick",ForwardRegionField::Q3VKick);
// static ForwardRegionMgField q4vkickA("Q4VKickA",ForwardRegionField::Q4VKickA);
// static ForwardRegionMgField q4hkick("Q4HKick",ForwardRegionField::Q4HKick);
// static ForwardRegionMgField q4vkickB("Q4VKickB",ForwardRegionField::Q4VKickB);
// static ForwardRegionMgField q5hkick("Q5HKick",ForwardRegionField::Q5HKick);
// static ForwardRegionMgField q6vkick("Q6VKick",ForwardRegionField::Q6VKick);

MagField::ForwardRegionFieldSvc::ForwardRegionFieldSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  IMagFieldSvc(),
  m_magnet(-1),
  m_magDataType(0),
  m_MQXA_DataFile(""), //"MQXA_NOMINAL.dat" if name = Q1 or Q3
  m_MQXB_DataFile(""), //"MQXB_NOMINAL.dat" if name = Q2
  m_properties("ForwardRegionProperties")
{

  declareProperty("Magnet", m_magnet, "integer property");
  declareProperty("MQXA_DataFile", m_MQXA_DataFile, "");
  declareProperty("MQXB_DataFile", m_MQXB_DataFile, "");
  declareProperty("ForwardRegionProperties", m_properties);
  m_Config.clear();

  if(!m_MQXA_DataFile.empty())
    {
      const int fileMQXACols(7); //TODO make this a property

      // load field map MQXA
      std::vector<std::vector<std::string> > loadedDataFile = loadDataFile(m_MQXA_DataFile.c_str(),fileMQXACols);

      ATH_MSG_DEBUG("Field map MQXA loaded");

      // initialize magnetic induction mesh
      for(int j=0; j < s_colsMQXA; j++)
        {
          for(int i=0; i < s_rowsMQXA; i++)
            {
              m_magIndMQXA[i][j][0] = atof(loadedDataFile[i+j*s_colsMQXA][4].c_str());
              m_magIndMQXA[i][j][1] = atof(loadedDataFile[i+j*s_colsMQXA][5].c_str());
            }
        }
    }

  if(!m_MQXB_DataFile.empty())
    {
      const int fileMQXBCols(7); //TODO make this a property

      // load field map MQXB
      std::vector<std::vector<std::string> > loadedDataFileB = loadDataFile(m_MQXB_DataFile.c_str(),fileMQXBCols);

      ATH_MSG_DEBUG("Field map MQXB loaded");

      // initialize magnetic induction mesh
      for(int j=0; j < s_colsMQXB; j++)
        {
          for(int i=0; i < s_rowsMQXB; i++)
            {
              m_magIndMQXB[i][j][0] = atof(loadedDataFileB[i+j*s_colsMQXB][4].c_str());
              m_magIndMQXB[i][j][1] = atof(loadedDataFileB[i+j*s_colsMQXB][5].c_str());
            }
        }
    }


}

StatusCode MagField::ForwardRegionFieldSvc::initialize()
{
  ATH_CHECK(m_properties.retrieve());

  ATH_MSG_INFO("Postponing magnet strengths initialization of " << this->name() << " till the begin of run");
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", this->name());
  ATH_CHECK(incidentSvc.retrieve());
  incidentSvc->addListener( this, IncidentType::BeginRun );
  ATH_MSG_INFO("Added listener to BeginRun incident");
  return StatusCode::SUCCESS;
}

// Query the interfaces - no idea what this is - TODO: find out
StatusCode MagField::ForwardRegionFieldSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IIncidentListener::interfaceID().versionMatch(riid))
    {
      *ppvInterface = dynamic_cast<IIncidentListener*>(this);
    }
  else
    {
     // Interface is not directly available: try out a base class
      return Service::queryInterface(riid, ppvInterface);
    }
  return StatusCode::SUCCESS;
}


// Handle incident function - if BeginRun happens, initialize mag. fields
void MagField::ForwardRegionFieldSvc::handle(const Incident& runIncident)
{
  ATH_MSG_INFO("handling incidents ..."); //FIXME drop to DEBUG level
  if (runIncident.type() == IncidentType::BeginRun)
    {
      InitMagData();
    }
  ATH_MSG_INFO("BeginRun incident handled");
}


/** IMagFieldSvc interface methods **/
/** get B field value at given position */
/** xyz[3] is in mm, bxyz[3] is in kT */
/** if deriv[9] is given, field derivatives are returned in kT/mm */
void MagField::ForwardRegionFieldSvc::getField(const double *xyz, double *bxyz, double*)
{
  G4ThreeVector f = this->FieldValue(G4ThreeVector(xyz[0],xyz[1],xyz[2]));
  bxyz[0] = f[0];
  bxyz[1] = f[1];
  bxyz[2] = f[2];
}

/** get B field value on the z-r plane at given position */
/** works only inside the solenoid; otherwise calls getField() above */
/** xyz[3] is in mm, bxyz[3] is in kT */
/** if deriv[9] is given, field derivatives are returned in kT/mm */
void MagField::ForwardRegionFieldSvc::getFieldZR(const double*, double*, double*)
{
  throw; //FIXME not supported yet.
}

G4ThreeVector MagField::ForwardRegionFieldSvc::FieldValue(G4ThreeVector Point) const
{
    int beam;
    if(Point[2] < 0) beam = 1;
    else beam = 2;


    HepGeom::Point3D<double> pointMagStart;
    HepGeom::Point3D<double> pointMagEnd;
    double rotZ = 0;

    getMagnetTransformParams(beam, m_magnet, Point, pointMagStart, pointMagEnd, rotZ);

    G4ThreeVector field;

    // are points defined (non-zero)? If not, only rotation around Z will be applied (if any)
    bool pointsNotDefined = (pointMagStart.distance2() == 0 || pointMagEnd.distance2() == 0);

    // calculate x and y shifts -- for quadrupoles only, no effect in dipoles (magnet borders to be solved in GeoModel)
    double xShift = pointsNotDefined ? getMagXOff(m_magnet) : (pointMagStart[0]+pointMagEnd[0])*0.5;
    double yShift = pointsNotDefined ? getMagYOff(m_magnet) : (pointMagStart[1]+pointMagEnd[1])*0.5;

    if(m_magnet <= Q3){ // inner triplet
        if(m_Config.bUseFLUKAMapsForInnerTriplet)
            field = getMagInd(Point,m_magnet, beam);
        else {
            double gradB = getMag(m_magnet,beam)*CLHEP::tesla/CLHEP::m*pow(-1.0,beam);
            field = G4ThreeVector(gradB*(Point[1]-yShift),gradB*(Point[0]-xShift),0);
        }
    }
    else if(m_magnet <= D2) // dipoles
        field = G4ThreeVector(0,getMag(m_magnet,beam)*CLHEP::tesla,0);
    else if(m_magnet <= Q7) // other quadrupoles
    {
        double gradB = getMag(m_magnet,beam)*CLHEP::tesla/CLHEP::m*pow(-1.0,beam);
        field = G4ThreeVector(gradB*(Point[1]-yShift),gradB*(Point[0]-xShift),0);
    }
    else // kickers
        return getKick(beam);

    // No points defined, check if there is rotation around magnet axis, else return unchanged field
    if(pointsNotDefined){
        if(rotZ == 0) return field;
        return HepGeom::RotateZ3D(rotZ)*(HepGeom::Vector3D<double>)field;
    }

    // Determine rotation from start and end points, shift is already taken care of
    HepGeom::Point3D<double> pointMagEndNoShift(pointMagEnd[0]-xShift, pointMagEnd[1]-yShift, pointMagEnd[2]);

    HepGeom::Point3D<double> pointMagRotCenter(0,0,(pointMagStart[2]+pointMagEnd[2])/2);

    HepGeom::Vector3D<double> vecNoRot(0,0,pointMagEnd[2]-pointMagRotCenter[2]);
    HepGeom::Vector3D<double> vecRot = pointMagEndNoShift - pointMagRotCenter;

    HepGeom::Vector3D<double> vecRotAxis = vecNoRot.cross(vecRot);
    double angle = vecNoRot.angle(vecRot);

    HepGeom::Transform3D rotateField = HepGeom::Rotate3D(angle, vecRotAxis);

    field = rotateField*HepGeom::RotateZ3D(rotZ)*(HepGeom::Vector3D<double>)field;
    return field;
}










// The main initialization - initialization of fields based on twiss files or magnets.dat
void MagField::ForwardRegionFieldSvc::InitMagData()
{
    ATH_MSG_INFO("Initializing magnetic field");

    m_Config = *(m_properties->getConf());

    if(m_Config.twissFileB1 == "" || m_Config.twissFileB2 == "" || m_Config.momentum == 0){
        m_magDataType = 0;

        ATH_MSG_INFO("Using magnets.dat as the field settings source");

        const std::string fileMagnets("magnets.dat");
        m_magnets = loadDataFile(fileMagnets.c_str(),5);
    }
    else
    {
        m_magDataType = 1;

        ATH_MSG_INFO("Using twiss files (" << m_Config.twissFileB1 << ", " << m_Config.twissFileB2 << ") as the field settings source");

        std::string headerB1;
        std::string headerB2;
        // Load twiss files and calculate field values for magnets
        std::vector<std::vector<std::string> > loadedTwissFileB1 = loadDataFileNLines(m_Config.twissFileB1.c_str(),30,200,headerB1);
        std::vector<std::vector<std::string> > loadedTwissFileB2 = loadDataFileNLines(m_Config.twissFileB2.c_str(),30,200,headerB2);

        // Beam 1
        InitMagDataFromTwiss(loadedTwissFileB1, 1, m_Config.momentum);

        // Beam 2
        InitMagDataFromTwiss(loadedTwissFileB2, 2, m_Config.momentum);

        //writeOutTwiss(loadedTwissFileB1,1,headerB1);
        //writeOutTwiss(loadedTwissFileB2,2,headerB2);
        ATH_MSG_INFO("Field initialized.");
    }
}

void MagField::ForwardRegionFieldSvc::InitMagDataFromTwiss(std::vector<std::vector<std::string> > loadedTwissFile, int beam, double momentum)
{
    int textIndex = 0;
    int lengthIndex = 4;
    int hkick = 5;
    int vkick = 6;
    int k0LIndex = 7;
    int k1LIndex = 8;

    bool dipole = false;
    bool quadrupole = false;
    bool kicker = false;
    double length;

    // init offsets (Q4--Q7 have dx=-97 mm, others 0), do not apply to kickers
    if(m_magnet <= Q7) {
        if(m_magnet >= Q4)
            m_magData[2] = -97*CLHEP::mm;
        else
            m_magData[2] = 0;

        m_magData[3] = 0;
    }

    for(int i=0; i < 150; i++)
    {
        // dipoles
        dipole =    (m_magnet == D1 && (loadedTwissFile[i][textIndex] == "\"MBXW.A4R1\"" || loadedTwissFile[i][textIndex] == "\"MBXW.A4L1\""))
                 || (m_magnet == D2 && (loadedTwissFile[i][textIndex] == "\"MBRC.4R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MBRC.4L1.B2\""));
        if(dipole){
            m_magData[beam-1] = kLToB(atof(loadedTwissFile[i][k0LIndex].c_str()), atof(loadedTwissFile[i][lengthIndex].c_str()), momentum);
            return;
        }

        //quadrupoles
        quadrupole =    (m_magnet == Q1 && (loadedTwissFile[i][textIndex] == "\"MQXA.1R1\"" || loadedTwissFile[i][textIndex] == "\"MQXA.1L1\""))
                 || (m_magnet == Q2 && (loadedTwissFile[i][textIndex] == "\"MQXB.A2R1\"" || loadedTwissFile[i][textIndex] == "\"MQXB.A2L1\""))
                 || (m_magnet == Q3 && (loadedTwissFile[i][textIndex] == "\"MQXA.3R1\"" || loadedTwissFile[i][textIndex] == "\"MQXA.3L1\""))
                 || (m_magnet == Q4 && (loadedTwissFile[i][textIndex] == "\"MQY.4R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MQY.4L1.B2\""))
                 || (m_magnet == Q5 && (loadedTwissFile[i][textIndex] == "\"MQML.5R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MQML.5L1.B2\""))
                 || (m_magnet == Q6 && (loadedTwissFile[i][textIndex] == "\"MQML.6R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MQML.6L1.B2\""))
                 || (m_magnet == Q7 && (loadedTwissFile[i][textIndex] == "\"MQM.A7R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MQM.A7L1.B2\""));
        if(quadrupole){
            m_magData[beam-1] = kLToB(atof(loadedTwissFile[i][k1LIndex].c_str()), atof(loadedTwissFile[i][lengthIndex].c_str()), momentum);
            return;
        }

        // kickers
        kicker =    (m_magnet == Q1HKick && (loadedTwissFile[i][textIndex] == "\"MCBXH.1R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXH.1L1\""))
                 || (m_magnet == Q1VKick && (loadedTwissFile[i][textIndex] == "\"MCBXV.1R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXV.1L1\""))
                 || (m_magnet == Q2HKick && (loadedTwissFile[i][textIndex] == "\"MCBXH.2R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXH.2L1\""))
                 || (m_magnet == Q2VKick && (loadedTwissFile[i][textIndex] == "\"MCBXV.2R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXV.2L1\""))
                 || (m_magnet == Q3HKick && (loadedTwissFile[i][textIndex] == "\"MCBXH.3R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXH.3L1\""))
                 || (m_magnet == Q3VKick && (loadedTwissFile[i][textIndex] == "\"MCBXV.3R1\"" || loadedTwissFile[i][textIndex] == "\"MCBXV.3L1\""))
                 || (m_magnet == Q4VKickA && (loadedTwissFile[i][textIndex] == "\"MCBYV.A4R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MCBYV.A4L1.B2\""))
                 || (m_magnet == Q4HKick && (loadedTwissFile[i][textIndex] == "\"MCBYH.4R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MCBYH.4L1.B2\""))
                 || (m_magnet == Q4VKickB && (loadedTwissFile[i][textIndex] == "\"MCBYV.B4R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MCBYV.B4L1.B2\""))
                 || (m_magnet == Q5HKick && (loadedTwissFile[i][textIndex] == "\"MCBCH.5R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MCBCH.5L1.B2\""))
                 || (m_magnet == Q6VKick && (loadedTwissFile[i][textIndex] == "\"MCBCV.6R1.B1\"" || loadedTwissFile[i][textIndex] == "\"MCBCV.6L1.B2\""));

        if(kicker){
            // length of Q1--Q3 kickers in twiss files is zero -> it is set to 0.001 m (also the length of corresponding GeoModel volumes)
            length = atof(loadedTwissFile[i][lengthIndex].c_str()) ? atof(loadedTwissFile[i][lengthIndex].c_str()) : 0.001;
            // calculation of the B from deflection angle --- beam going down at the IP
            m_magData[beam-1] = pow(-1.0, beam+1)*kLToB(atof(loadedTwissFile[i][vkick].c_str()), length, momentum);
            m_magData[beam+1] = kLToB(atof(loadedTwissFile[i][hkick].c_str()), length, momentum);
            return;
        }
    }
}


/** Non-inherited public methods FIXME - add new interface? */
// get magnetic induction vector in certain point inside MQXA (MQXAB = 0) or MQXB (MQXAB = 1) (uses bilinear interpolation), q=0 - Q1, q=1 - Q2a, q=2 - Q2b, q=3 - Q3
G4ThreeVector MagField::ForwardRegionFieldSvc::getMagInd(G4ThreeVector Point, int q, int beam) const
{
  int MQXAB = 0;
  if(q == 1) MQXAB = 1;

  //magnet gradient and offsets
  double gradMQX = getMag(q,beam);
  double xOffMQX = getMagXOff(q);
  double yOffMQX = getMagYOff(q);

  //mapfile parameters
  double minMQXA = -24*CLHEP::cm;
  double maxMQXA = 24*CLHEP::cm;
  double minMQXB = -26*CLHEP::cm;
  double maxMQXB = 26*CLHEP::cm;

  double x,y,x1,y1,x2,y2,xstep,ystep;
  int i1,i2,j1,j2;

  G4ThreeVector B(0,0,0);
  if(MQXAB == 0) // field in MQXA
    {
      x = Point[0] - xOffMQX;
      y = Point[1] - yOffMQX;

      xstep = (maxMQXA - minMQXA)/(s_rowsMQXA-1);
      ystep = (maxMQXA - minMQXA)/(s_colsMQXA-1);


      i2 = ceil(x/xstep)+ceil(s_rowsMQXA/2.);
      j2 = ceil(y/ystep)+ceil(s_colsMQXA/2.);
      i1 = i2 - 1;
      j1 = j2 - 1;
      x2 = (i2-ceil(s_rowsMQXA/2.))*xstep;
      y2 = (j2-ceil(s_colsMQXA/2.))*ystep;
      x1 = x2 - xstep;
      y1 = y2 - ystep;

      if(x < maxMQXA && x > minMQXA && y < maxMQXA && y > minMQXA) // field in MQXA
        {
          B[0] = (m_magIndMQXA[i1][j1][0]*(x2-x)*(y2-y) + m_magIndMQXA[i2][j1][0]*(x-x1)*(y2-y) + m_magIndMQXA[i1][j2][0]*(x2-x)*(y-y1) + m_magIndMQXA[i2][j2][0]*(x-x1)*(y-y1))/xstep/ystep*CLHEP::tesla;
          B[1] = (m_magIndMQXA[i1][j1][1]*(x2-x)*(y2-y) + m_magIndMQXA[i2][j1][1]*(x-x1)*(y2-y) + m_magIndMQXA[i1][j2][1]*(x2-x)*(y-y1) + m_magIndMQXA[i2][j2][1]*(x-x1)*(y-y1))/xstep/ystep*CLHEP::tesla;
          B[0] = B[0]*pow(-1.0,beam)*(gradMQX/224.29);
          B[1] = B[1]*pow(-1.0,beam)*(gradMQX/224.29);
        }
    }
  if(MQXAB == 1) // field in MQXB
    {
      x = Point[0] - xOffMQX;
      y = Point[1] - yOffMQX;

      xstep = (maxMQXB - minMQXB)/(s_rowsMQXB-1);
      ystep = (maxMQXB - minMQXB)/(s_colsMQXB-1);

      i2 = ceil(x/xstep)+ceil(s_rowsMQXB/2);
      j2 = ceil(y/ystep)+ceil(s_colsMQXB/2);
      i1 = i2 - 1;
      j1 = j2 - 1;
      x2 = (i2-ceil(s_rowsMQXB/2))*xstep;
      y2 = (j2-ceil(s_colsMQXB/2))*ystep;
      x1 = x2 - xstep;
      y1 = y2 - ystep;

      if(x < maxMQXB && x > minMQXB && y < maxMQXB && y > minMQXB) // field in MQXB
        {
          B[0] = (m_magIndMQXB[i1][j1][0]*(x2-x)*(y2-y) + m_magIndMQXB[i2][j1][0]*(x-x1)*(y2-y) + m_magIndMQXB[i1][j2][0]*(x2-x)*(y-y1) + m_magIndMQXB[i2][j2][0]*(x-x1)*(y-y1))/xstep/ystep*CLHEP::tesla;
          B[1] = (m_magIndMQXB[i1][j1][1]*(x2-x)*(y2-y) + m_magIndMQXB[i2][j1][1]*(x-x1)*(y2-y) + m_magIndMQXB[i1][j2][1]*(x2-x)*(y-y1) + m_magIndMQXB[i2][j2][1]*(x-x1)*(y-y1))/xstep/ystep*CLHEP::tesla;
          B[0] = B[0]*pow(-1.0,beam)*(gradMQX/216.1787104);
          B[1] = B[1]*pow(-1.0,beam)*(gradMQX/216.1787104);
        }
    }
  return B;
}

double MagField::ForwardRegionFieldSvc::getMag(int magnet, int beam) const
{
  if(m_magDataType == 1) return m_magData[beam-1];
  return atof(m_magnets[magnet][beam].c_str());
}

G4ThreeVector MagField::ForwardRegionFieldSvc::getKick(int beam) const
{
  if(m_magDataType == 1) return G4ThreeVector(m_magData[beam-1]*CLHEP::tesla,m_magData[beam+1]*CLHEP::tesla,0);
  return G4ThreeVector(0,0,0); // magnets.dat does not contain kickers
}

double MagField::ForwardRegionFieldSvc::getMagXOff(int magnet) const
{
  if(m_magDataType == 1) return m_magData[2];
  return atof(m_magnets[magnet][3].c_str())*CLHEP::mm;
}

double MagField::ForwardRegionFieldSvc::getMagYOff(int magnet) const
{
  if(m_magDataType == 1) return m_magData[3];
  return atof(m_magnets[magnet][4].c_str())*CLHEP::mm;
}

// Load data from file into 2D array of strings. Input is filename, wanted number of columns and max number of lines (in original file) to be loaded (N=0 means all)
std::vector<std::vector<std::string> > MagField::ForwardRegionFieldSvc::loadDataFileNLines(const char * fileName, int cols, int N, std::string& header)
{
    std::vector<std::vector<std::string> > loadedData;

    header = "";

    MsgStream LogStream(Athena::getMessageSvc(), "MagField::ForwardRegionFieldSvc::loadDataFile()");

    std::ifstream file (fileName);
    if(!file){
        std::string datapath = PathResolver::find_file(fileName,"DATAPATH", PathResolver::RecursiveSearch);
        LogStream << MSG::DEBUG << "File " << fileName << " not found in run directory, trying to load it from DATAPATH" << endreq;
        file.open(datapath.c_str());
    }

    if(!file)
        LogStream << MSG::FATAL << "Unable to load " << fileName << endreq;

    if(file.is_open())
    {
        std::vector<std::string> row (cols);
        char c;
        char temp[1024];

        bool unlimited = false;
        if(N==0) unlimited = true;
        int i = 0;
        while(file.get(c) && (i < N || unlimited))
        {
            if(c != '@' && c != '#' && c != '*' && c != '$' && c != '%' && c!='\n')
            {
                file.unget();
                for(int i = 0; i<cols; i++) // load desired columns
                {
                    file >> row[i];
                }
                loadedData.push_back(row); //store them
                file.ignore(1024,'\n'); // discard rest of line
            }
            else if(c == '@')
            {
              file.unget();
              file.getline(temp,1024);
              header.append(temp);
              header.append("\n");
            }
            else if(c == '*')
            {
              file.unget();
              file.getline(temp,1024);
              header.append(temp);
              header.append("                                     PosXStart");
              header.append("          PosYStart");
              header.append("          PosZStart");
              header.append("          PosXEnd  ");
              header.append("          PosYEnd  ");
              header.append("          PosZEnd  ");
              header.append("          RotZ");
              header.append("\n");
            }
            else if(c == '$')
            {
              file.unget();
              file.getline(temp,1024);
              header.append(temp);
              header.append("                                        %le    ");
              header.append("             %le    ");
              header.append("             %le    ");
              header.append("             %le    ");
              header.append("             %le    ");
              header.append("             %le    ");
              header.append("             %le    ");
              header.append("\n");
            }
            else
              file.ignore(1024, '\n'); // discard commented lines
            i++;
        }
        LogStream << MSG::INFO << "File " << fileName << " succesfully loaded." << endreq;
        file.close();
    }
    return loadedData;
}

std::vector<std::vector<std::string> > MagField::ForwardRegionFieldSvc::loadDataFile(const char * fileName, int cols)
{
    std::string header;
    return loadDataFileNLines(fileName, cols, 0, header);
}

// Calculate magnetic induction value / gradient from K*L values, same relation holds for induction value from deviation angle (hkick, vkick)
double MagField::ForwardRegionFieldSvc::kLToB(double kL, double length, double momentum)
{
    return kL*momentum/length/0.299792458;
}

void MagField::ForwardRegionFieldSvc::writeOutTwiss(std::vector<std::vector<std::string> > loadedTwissFile, int beam, std::string header)
{
    MsgStream LogStream(Athena::getMessageSvc(), "MagField::ForwardRegionFieldSvc::writeOutTwiss()");


    std::ostringstream fileName;
    fileName << "alfaTwiss" << beam << ".txt";
    std::ofstream file (fileName.str().c_str());
    int magID;
    HepGeom::Point3D<double> pointMagStart;
    HepGeom::Point3D<double> pointMagEnd;
    double rotZ = 0;
    double PointZ;
    G4ThreeVector Point;

    if(!file)
      LogStream << MSG::ERROR << "Unable to write to " << fileName.str() << endreq;

    if(file.is_open())
    {
        file << header;
        int fileSize = loadedTwissFile.size();
        int rowSize = 0;
        for(int i=0; i < fileSize;i++)
        {
            rowSize = loadedTwissFile[i].size();
            for(int j=0; j<rowSize; j++)
            {
                if(j < 3) file << std::left;
                else file << std::right;
                file << std::setw(20) << loadedTwissFile[i][j];
            }
            magID = getMagNumFromName(loadedTwissFile[i][0]);
            if(magID >= 0){
                PointZ = atof(loadedTwissFile[i][3].c_str())*CLHEP::m;
                Point.set(0,0,PointZ);
                getMagnetTransformParams(beam, magID, Point, pointMagStart, pointMagEnd, rotZ);
                file << std::setw(20) << pointMagStart[0]/CLHEP::m;
                file << std::setw(20) << pointMagStart[1]/CLHEP::m;
                file << std::setw(20) << pointMagStart[2]/CLHEP::m;
                file << std::setw(20) << pointMagEnd[0]/CLHEP::m;
                file << std::setw(20) << pointMagEnd[1]/CLHEP::m;
                file << std::setw(20) << pointMagEnd[2]/CLHEP::m;
                file << std::setw(20) << rotZ;
            }
            else {
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
                file << std::setw(20) << 0;
            }

            file << std::endl;
        }
    }
    file.close();
}

int MagField::ForwardRegionFieldSvc::getMagNumFromName(std::string name) const
{
    // dipoles
    if (name == "\"MBXW.A4R1\"" || name == "\"MBXW.A4L1\"") return D1;
    if (name == "\"MBXW.B4R1\"" || name == "\"MBXW.B4L1\"") return D1;
    if (name == "\"MBXW.C4R1\"" || name == "\"MBXW.C4L1\"") return D1;
    if (name == "\"MBXW.D4R1\"" || name == "\"MBXW.D4L1\"") return D1;
    if (name == "\"MBXW.E4R1\"" || name == "\"MBXW.E4L1\"") return D1;
    if (name == "\"MBXW.F4R1\"" || name == "\"MBXW.F4L1\"") return D1;
    if (name == "\"MBRC.4R1.B1\"" || name == "\"MBRC.4L1.B2\"") return D2;

    //quadrupoles
    if(name == "\"MQXA.1R1\"" || name == "\"MQXA.1L1\"") return Q1;
    if(name == "\"MQXB.A2R1\"" || name == "\"MQXB.A2L1\"") return Q2;
    if(name == "\"MQXB.B2R1\"" || name == "\"MQXB.B2L1\"") return Q2;
    if(name == "\"MQXA.3R1\"" || name == "\"MQXA.3L1\"") return Q3;
    if(name == "\"MQY.4R1.B1\"" || name == "\"MQY.4L1.B2\"") return Q4;
    if(name == "\"MQML.5R1.B1\"" || name == "\"MQML.5L1.B2\"") return Q5;
    if(name == "\"MQML.6R1.B1\"" || name == "\"MQML.6L1.B2\"") return Q6;
    if(name == "\"MQM.A7R1.B1\"" || name == "\"MQM.A7L1.B2\"") return Q7;
    if(name == "\"MQM.B7R1.B1\"" || name == "\"MQM.B7L1.B2\"") return Q7;

    // kickers
    if(name == "\"MCBXH.1R1\"" || name == "\"MCBXH.1L1\"") return Q1HKick;
    if(name == "\"MCBXV.1R1\"" || name == "\"MCBXV.1L1\"") return Q1VKick;
    if(name == "\"MCBXH.2R1\"" || name == "\"MCBXH.2L1\"") return Q2HKick;
    if(name == "\"MCBXV.2R1\"" || name == "\"MCBXV.2L1\"") return Q2VKick;
    if(name == "\"MCBXH.3R1\"" || name == "\"MCBXH.3L1\"") return Q3HKick;
    if(name == "\"MCBXV.3R1\"" || name == "\"MCBXV.3L1\"") return Q3VKick;
    if(name == "\"MCBYV.A4R1.B1\"" || name == "\"MCBYV.A4L1.B2\"") return Q4VKickA;
    if(name == "\"MCBYH.4R1.B1\"" || name == "\"MCBYH.4L1.B2\"") return Q4HKick;
    if(name == "\"MCBYV.B4R1.B1\"" || name == "\"MCBYV.B4L1.B2\"") return Q4VKickB;
    if(name == "\"MCBCH.5R1.B1\"" || name == "\"MCBCH.5L1.B2\"") return Q5HKick;
    if(name == "\"MCBCV.6R1.B1\"" || name == "\"MCBCV.6L1.B2\"") return Q6VKick;
    return -1;
}

void MagField::ForwardRegionFieldSvc::getMagnetTransformParams(int beam, int magnet, G4ThreeVector Point, HepGeom::Point3D<double> &pointMagStart,HepGeom::Point3D<double> &pointMagEnd, double &rotZ) const
{
    // find out which magnet we are in and get corresponding displacements
    switch(magnet){
    case Q1:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ1Start[(beam-1)*3],m_Config.pointQ1Start[(beam-1)*3+1],m_Config.pointQ1Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ1End[(beam-1)*3],m_Config.pointQ1End[(beam-1)*3+1],m_Config.pointQ1End[(beam-1)*3+2]);
        rotZ = m_Config.fQ1RotZ[beam-1];
        break;
    case Q2:
      if(std::abs(Point[2]) < 38*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ2aStart[(beam-1)*3],m_Config.pointQ2aStart[(beam-1)*3+1],m_Config.pointQ2aStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ2aEnd[(beam-1)*3],m_Config.pointQ2aEnd[(beam-1)*3+1],m_Config.pointQ2aEnd[(beam-1)*3+2]);
            rotZ = m_Config.fQ2aRotZ[beam-1];
        }
        else
        {
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ2bStart[(beam-1)*3],m_Config.pointQ2bStart[(beam-1)*3+1],m_Config.pointQ2bStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ2bEnd[(beam-1)*3],m_Config.pointQ2bEnd[(beam-1)*3+1],m_Config.pointQ2bEnd[(beam-1)*3+2]);
            rotZ = m_Config.fQ2bRotZ[beam-1];
        }
        break;
    case Q3:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ3Start[(beam-1)*3],m_Config.pointQ3Start[(beam-1)*3+1],m_Config.pointQ3Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ3End[(beam-1)*3],m_Config.pointQ3End[(beam-1)*3+1],m_Config.pointQ3End[(beam-1)*3+2]);
        rotZ = m_Config.fQ3RotZ[beam-1];
        break;
    case Q4:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ4Start[(beam-1)*3],m_Config.pointQ4Start[(beam-1)*3+1],m_Config.pointQ4Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ4End[(beam-1)*3],m_Config.pointQ4End[(beam-1)*3+1],m_Config.pointQ4End[(beam-1)*3+2]);
        rotZ = m_Config.fQ4RotZ[beam-1];
        break;
    case Q5:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ5Start[(beam-1)*3],m_Config.pointQ5Start[(beam-1)*3+1],m_Config.pointQ5Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ5End[(beam-1)*3],m_Config.pointQ5End[(beam-1)*3+1],m_Config.pointQ5End[(beam-1)*3+2]);
        rotZ = m_Config.fQ5RotZ[beam-1];
        break;
    case Q6:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ6Start[(beam-1)*3],m_Config.pointQ6Start[(beam-1)*3+1],m_Config.pointQ6Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ6End[(beam-1)*3],m_Config.pointQ6End[(beam-1)*3+1],m_Config.pointQ6End[(beam-1)*3+2]);
        rotZ = m_Config.fQ6RotZ[beam-1];
        break;
    case Q7:
        if(std::abs(Point[2]) < 263.5*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ7aStart[(beam-1)*3],m_Config.pointQ7aStart[(beam-1)*3+1],m_Config.pointQ7aStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ7aEnd[(beam-1)*3],m_Config.pointQ7aEnd[(beam-1)*3+1],m_Config.pointQ7aEnd[(beam-1)*3+2]);
            rotZ = m_Config.fQ7aRotZ[beam-1];
        }
        else
        {
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ7bStart[(beam-1)*3],m_Config.pointQ7bStart[(beam-1)*3+1],m_Config.pointQ7bStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ7bEnd[(beam-1)*3],m_Config.pointQ7bEnd[(beam-1)*3+1],m_Config.pointQ7bEnd[(beam-1)*3+2]);
            rotZ = m_Config.fQ7bRotZ[beam-1];
        }
        break;
    case D1:
        if(std::abs(Point[2]) < 63.5*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1aStart[(beam-1)*3],m_Config.pointD1aStart[(beam-1)*3+1],m_Config.pointD1aStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1aEnd[(beam-1)*3],m_Config.pointD1aEnd[(beam-1)*3+1],m_Config.pointD1aEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1aRotZ[beam-1];
        }
        else if(std::abs(Point[2]) < 67.5*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1bStart[(beam-1)*3],m_Config.pointD1bStart[(beam-1)*3+1],m_Config.pointD1bStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1bEnd[(beam-1)*3],m_Config.pointD1bEnd[(beam-1)*3+1],m_Config.pointD1bEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1bRotZ[beam-1];
        }

        else if(std::abs(Point[2]) < 72*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1cStart[(beam-1)*3],m_Config.pointD1cStart[(beam-1)*3+1],m_Config.pointD1cStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1cEnd[(beam-1)*3],m_Config.pointD1cEnd[(beam-1)*3+1],m_Config.pointD1cEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1cRotZ[beam-1];
        }

        else if(std::abs(Point[2]) < 76*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1dStart[(beam-1)*3],m_Config.pointD1dStart[(beam-1)*3+1],m_Config.pointD1dStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1dEnd[(beam-1)*3],m_Config.pointD1dEnd[(beam-1)*3+1],m_Config.pointD1dEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1dRotZ[beam-1];
        }

        else if(std::abs(Point[2]) < 80.5*CLHEP::m){
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1eStart[(beam-1)*3],m_Config.pointD1eStart[(beam-1)*3+1],m_Config.pointD1eStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1eEnd[(beam-1)*3],m_Config.pointD1eEnd[(beam-1)*3+1],m_Config.pointD1eEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1eRotZ[beam-1];
        }

        else {
            pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1fStart[(beam-1)*3],m_Config.pointD1fStart[(beam-1)*3+1],m_Config.pointD1fStart[(beam-1)*3+2]);
            pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1fEnd[(beam-1)*3],m_Config.pointD1fEnd[(beam-1)*3+1],m_Config.pointD1fEnd[(beam-1)*3+2]);
            rotZ = m_Config.fD1fRotZ[beam-1];
        }
        break;
    case D2:
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD2Start[(beam-1)*3],m_Config.pointD2Start[(beam-1)*3+1],m_Config.pointD2Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD2End[(beam-1)*3],m_Config.pointD2End[(beam-1)*3+1],m_Config.pointD2End[(beam-1)*3+2]);
        rotZ = m_Config.fD2RotZ[beam-1];
        break;
    }
}
