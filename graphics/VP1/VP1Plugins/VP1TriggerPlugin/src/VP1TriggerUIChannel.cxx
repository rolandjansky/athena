/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerPlugins
 * @Class   : VP1TriggerUIChannel
 *
 * @brief   : Trigger UI to query trigger data utilizing the VP1TriggerTool
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerPlugin/VP1TriggerUIChannel.h"
#include "VP1TriggerSystems/VP1TriggerUISystem.h"
#include "VP1TriggerSystems/VP1TriggerTool.h"
#include "ui_triggeruiwidgetform.h"

//Global flags
bool dataAvailable=false;


//Trigger Channel Constructor
//_____________________________________________________________________________________________
VP1TriggerUIChannel::VP1TriggerUIChannel()
  : Logger("VP1TriggerUIChannel"),
    IVP1ChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1TriggerUIChannel,"Trigger"),
		      "Trigger UI to query trigger data utilizing the VP1TriggerTool",
		      "Manuel Proissl, mproissl@cern.ch"),
    sys(0),
    m_treeWidget(0),
    m_filterDisplay(0),
    m_searchBar(0),
    m_comboBox(0),
    m_switchButton(0),
    m_browserLabel(0)
{log_verbose("constructor");}


//Trigger Channel Destructor
//_____________________________________________________________________________________________
VP1TriggerUIChannel::~VP1TriggerUIChannel() {log_verbose("destructor");}


//System call: Initialization of VP1TriggerTool
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::init()
{
  log_info("Initializing VP1Trig::VP1TriggerUISystem");
  sys = new VP1TriggerUISystem();
  registerSystem(sys);
}//END: init


//System call: Setup GUI and (optional) set processing features of VP1TriggerTool
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::create()
{
  log_info("Init UI and setup initial config");
  
  Ui::TriggerUIWidgetForm ui;
  ui.setupUi(this);
  
  //Setup treeWidget
  m_treeWidget = ui.treeWidget;
  m_treeWidget->setColumnCount(1);
  m_treeWidget->setVisible(false);
  m_browserLabel = ui.browserLabel;
  m_browserLabel->setVisible(false);
  
  //Setup filterDisplay
  m_filterDisplay = ui.filterDisplay;
  m_filterDisplay->setReadOnly(true); //only display, no edit
  m_searchBar = ui.searchBar;
  m_searchBar->setEnabled(false);
  connect(m_searchBar,SIGNAL(textChanged(QString)),this,SLOT(filterDisplayUpdate(QString)));
  
  //Setup comboBox (preset filter)
  m_comboBox = ui.comboBox;
  m_comboBox->setEnabled(false);
  connect(m_comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setTrigLvl(QString)));
  
  //Setup switchButton (treeWidget<->textDisplay)
  m_switchButton = ui.switchButton;
  m_switchButton->setEnabled(false);
  m_switchButton->setCheckable(true);
  connect(m_switchButton,SIGNAL(toggled(bool)),this,SLOT(toggleDisplay()));
  
  //Initial trigger tool settings
  sys->m_triggertool->setDataContainerFormat('q');
}//END: create


//SLOT: Toggle: Turns Tree Browser On/Off
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::toggleDisplay()
{
  if(m_switchButton->isChecked()) {
    m_treeWidget->setVisible(true);
    m_browserLabel->setVisible(true);
  } else {
    m_treeWidget->setVisible(false);
    m_browserLabel->setVisible(false);
  }
}//END: toggleDisplay


//SLOT: ComboBox: Set Trigger Level and reload data to query
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::setTrigLvl(QString triglvl)
{ 
  //Disable controls
  m_searchBar->setEnabled(false);
  m_comboBox->setEnabled(false);
  m_switchButton->setEnabled(false);
  
  //Reload data set to tree
  if(triglvl=="All")
    loadTrigData(triglvl);
  else if(triglvl=="L1 Data")
    loadTrigData("L1");
  else if(triglvl=="L2 Data")
    loadTrigData("L2");
  else if(triglvl=="EF Data")
    loadTrigData("EF");
  else
    log_error("Developer Error: invalid item name in QComboBox!");
}//END: setTrigLvl


//SLOT: Switch Board for search commands
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::filterDisplayUpdate(QString search)
{
  //VARs
  bool cmd=false;
  QString instructions = "<b>Instructions:</b><br><br>"
                         "Utilize one of the filter options below to display trigger data:<br>"
                         "+ Enter any combination of filter strings, e.g. 'ef data spec track-0 pos'<br>"
                         "+ Choose from the drop-down menu to display either L1, L2 or EF data<br>"
                         "+ Use toggle button to display trigger data browser<br><br>"
                         "Furthermore, one can use a set of commands to retrieve trigger information.<br>"
                         "To enter a command, use the suffix: > <br>"
                         "For a list of commands, type: >help";
  
  if(search!="") {
    //Remove surrounding whitespace
    search=search.simplified();
    
    //Search string is command
    if(search.at(0)==QChar('>')) {
      search.remove(0,1);
      search=search.simplified();
      cmd=true;
    }
    
    if(cmd) { //---------------------------------------------->
      //*** Instructions:
      if(search.compare("info",Qt::CaseInsensitive)==0) {
	printToScreen(instructions);
      } //*** Help:
      else if(search.compare("help",Qt::CaseInsensitive)==0) {
	QString helpintro = "<b>Trigger UI Help</b><br><br>This tool provides filter options to query trigger data. One can either use the search bar and the drop down menu to filter as well as the browser to click through the data tree. The search bar also accepts a set of commands, which can be used through the suffix: > <br>";
	QString cmdlist = "<table border='0' width='60%' cellpadding='3' cellspacing='3'>"
                          "<tr bgcolor='#C8C8C8'><td>help</td><td>Displays this help message</td></tr>"
                          "<tr bgcolor='#E0E0E0'><td>info</td><td>Displays the user instructions</td></tr>"
	                  "<tr bgcolor='#C8C8C8'><td>muoncount</td><td>Show the number of muons in the event</td></tr></table>";
	
	printToScreen(helpintro+cmdlist);
      }
      else if(search.compare("muoncount",Qt::CaseInsensitive)==0) {
	int muoncount = sys->m_triggertool->getMuonCount();
	printToScreen("Number of muons in the event: <b>"+QString::number(muoncount)+"</b>");
      }
      else //*** Commands being typed:
	printToScreen("<b>Listening to command being entered...</b><br><br>Please check (> help) for a list of commands.");      
    }//END: commands -----------------------------------------<
    else {
      searchData(search);
    }//END: search strings
  }//search not empty
  else
    printToScreen(instructions);
}//END: filterDisplayUpdate


//Print any text to Filter Display
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::printToScreen(QString text)
{
  //Design Structure
  QString header = "<html><head><link rel='stylesheet' type='text/css' href='format.css'></head><body>", footer = "</body></html>";
  QString css = "#design { font-family: Courier New; font-size: 12px; margin: 0px; width: 100%; text-align: left; } #design th { font-size: 13px; font-weight: normal; padding: 2px; background: #000; border-top: 4px solid #000; border-bottom: 1px solid #fff; color: #fff; } #design td { padding: 2px; background: #ccc; border-bottom: 1px solid #fff; color: #000; border-top: 1px solid transparent; } .HL { background: #ffff00; color: #000; }";

  QTextDocument *doc = new QTextDocument;
  doc->addResource( QTextDocument::StyleSheetResource, QUrl("format.css"), css );
  doc->setHtml(header+text+footer);
  //doc->setPlainText(header+body+footer);
  m_filterDisplay->setDocument(doc);
  m_filterDisplay->show();
}


//System call: After new Event loaded: Retrieve trigger data and load to TreeWidget
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::systemRefreshed(IVP1System*)
{
  log_verbose("systemRefreshed()");
  
  //Retrieve System Status flags
  bool isReady = sys->m_triggertool->isReady();
  QString trigLvl = sys->m_triggertool->getTrigLvlToProcess();
  char dataFormat = sys->m_triggertool->getDataContainerFormat();
  
  if(isReady && dataFormat=='q') {
    QString currentSel = m_comboBox->currentText();
    if(currentSel!="All")
      setTrigLvl(currentSel);
    else
      loadTrigData(trigLvl);
  }
  else
    log_error("VP1TriggerTool not ready for DAQ. Setup terminated!");
}//END: systemRefreshed


//Load Trigger Data to QTreeWidget and Search Vectors
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::loadTrigData(QString triglvl)
{
  //Load trigger data from trigger tool
  QList<QTreeWidgetItem *> qtrigdata = sys->m_triggertool->getTriggerData_QTree(triglvl);
  
  //Display instructions
  filterDisplayUpdate(">info");
  
  if(int(qtrigdata.size())>0) {
    int topLvlItems = int(m_treeWidget->topLevelItemCount());
    if(topLvlItems>0) {
      //Clean up
      m_searchBar->clear();
      for(int i=0; i<=topLvlItems; ++i)
	m_treeWidget->takeTopLevelItem(i);

      //Verify clean up (Temp. fix of Qt problem)
      topLvlItems = int(m_treeWidget->topLevelItemCount());
      if(topLvlItems!=0) {
	for(int i=0; i<int(m_treeWidget->topLevelItemCount()); ++i)
	  m_treeWidget->takeTopLevelItem(i);
      }
      topLvlItems = int(m_treeWidget->topLevelItemCount());
      if(topLvlItems!=0)
	log_warning("Data tree still contains uncleared elements from previous loading.");
    }
    
    //Fill QTree with trigger data
    for(int i=0; i<int(qtrigdata.size()); ++i)
      m_treeWidget->insertTopLevelItem(i,qtrigdata[i]);
    
    //Load data pattern for search
    dataAvailable = loadDataPattern();
    
    if(dataAvailable) {
      m_searchBar->setEnabled(true);
      m_comboBox->setEnabled(true);
      m_switchButton->setEnabled(true);
    } else {
      log_error("Loading data pattern for search failed!");
      m_searchBar->setEnabled(false);
      m_comboBox->setEnabled(false);
      m_switchButton->setEnabled(false);
    }
  }
  else {
    log_error("Trigger Data QTree empty!");
    dataAvailable=false;
    m_searchBar->setEnabled(false);
    m_comboBox->setEnabled(false);
    m_switchButton->setEnabled(false);
  }
}//END: loadTrigData


//Load Trigger Data to Filtering Vectors (from QTreeWidget)
//_____________________________________________________________________________________________
bool VP1TriggerUIChannel::loadDataPattern()
{  
  QTreeWidgetItemIterator it_nodeset(m_treeWidget, QTreeWidgetItemIterator::HasChildren);
  QTreeWidgetItemIterator it_treeset(m_treeWidget);
  
  std::vector<QString> nodeset, pathitem(4), j_trigData;
  QString item, pathstr, _pathstr="";
  int level=0, hasChildren=0, isNode=0, id=0;
  
  //Reset containers
  trigNode.clear();
  trigData.clear();
  
  //Fill nodeset vector
  while(*it_nodeset) {
    nodeset.push_back((*it_nodeset)->text(0));
    ++it_nodeset;
  }
  
  //Safety check
  if(int(nodeset.size())==0) {
    log_error("Iteration over node set returned zero elements!");
    return false;
  }  
  
  //Fill treeset vector
  while (*it_treeset) {
    item = (*it_treeset)->text(0);
    
    //Define structure to solve hiercharchy problem
    if(item=="L1 Data" || item=="L2 Data" || item=="EF Data") {
      level=0; pathitem[level]=item;
    }
    else if(item=="Triggers" || item=="Barrel" || item=="Endcap" || item=="General" || item.contains("Muon No")) {
      level=1; pathitem[level]=item;
    }
    else if(item.contains("TE:") || item.contains("Passed") || item.contains("Muon Type")) {
      level=2; pathitem[level]=item;
    }
    else if(item.contains("Track-") || item.contains("ROI_")) {
      level=3; pathitem[level]=item;
    }
    else {
      //Verify: hasChildren and isNode
      for(int i=0; i < int(nodeset.size()); ++i) {
        if(pathitem[level]==nodeset[i]) hasChildren++;
        if(item==nodeset[i]) isNode++;
      }
      //Load data
      if(hasChildren>0 && isNode==0) {
        for(int j=0; j <= level; ++j) {
          pathstr.append(pathitem[j] + " > ");
        }
        if(_pathstr!=pathstr) {
          if(id!=0) {
            trigData.push_back(j_trigData); j_trigData.clear();
            trigNode << pathstr;
            j_trigData.push_back(item);
          } else {
            trigNode << pathstr;
            j_trigData.push_back(item);
          }
          ++id;
	  _pathstr=pathstr;
        }
        else
          j_trigData.push_back(item);
	
        pathstr.clear();
      }
    }
    ++it_treeset;
  } if(int(trigNode.size())!=int(trigData.size()) && int(j_trigData.size())>0) 
      trigData.push_back(j_trigData);
  
  if(int(trigNode.size())==int(trigData.size()) && int(trigNode.size())>0)
    return true;
  log_error("Data pattern loading unsuccessful!");
  return false;
}//END: loadDataPattern


//Trigger Data Filter: Algorithm, Design and Display
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::searchData(QString display)
{
  //Main Variables
  int segMatchNode=0, segMatchData=0, _idx=0, hl_b=0, hl_e=0, hl_check=0, hl_idx=0;
  QStringList displaySeg, pathSeg, dataSeg, foundSeg, compSeg;
  QString body, path, hl_trigNode, hl_trigData;
  QList<int> retId, dataFlag, hlSetNode, hlSetData;
  QList<QString> retContainer;
  bool newTable=true;
  bool displayResult=true;
  
  //Design Structure
  QString table_b = "<table id='design'>", table_e = "</table><br><br>";
  QString thead_b = "<thead><tr><th>", thead_e = "</th></tr></thead>";
  QString tbody_b = "<tbody>", tbody_e = "</tbody>";
  QString data_b = "<tr><td>", data_e = "</td></tr>";
  QString highlight_b = "<font class='HL'>", highlight_e = "</font>";
  
  if(trigData.size()!=0 && dataAvailable) { //data available
    if(display!="") {
      display = display.simplified();
      displaySeg = display.split(QRegExp("\\s+"));
    }
    if(displaySeg.size()>0) {
      //Duplicate search strings
      displaySeg.removeDuplicates();
      display = displaySeg.join(" ");
    }
    
    //Search trigger containers
    if(trigData[0].size()>0 && display!="" && display!="All") {
      retContainer << "<b>FILTERING TRIGGER DATA :: " + display + "</b><br><br>";
      
      //Retrieve collection indices
      for (int i=0; i < int(trigNode.size()); ++i) {
        for (int j=0; j < int(trigData[i].size()); ++j) {
          for (int k=0; k < int(displaySeg.size()); ++k) {
            if(trigNode[i].contains(displaySeg[k], Qt::CaseInsensitive) && j==0) { //check once
              foundSeg << displaySeg[k];
              ++segMatchNode;
            }
            if(trigData[i][j].contains(displaySeg[k], Qt::CaseInsensitive)) {
              foundSeg << displaySeg[k];
              ++segMatchData;
            }
          }
          dataFlag << j << segMatchData; // (id,count)
          segMatchData=0;
        }
	//---------------------------------------------------
        //+ Check collection contains all segments
        foundSeg.removeDuplicates();
        compSeg = displaySeg;
        compSeg.sort(); foundSeg.sort();
	
        if(foundSeg==compSeg && foundSeg.size()!=0) {
          //+ Query entire collection
          for (int x=0; x < int(dataFlag.size()); x+=2) {
            if(dataFlag[x+1]!=0)
              ++segMatchData;
          }
          //+ Case A: all segments in node, none in data
          if(segMatchNode==displaySeg.size() && segMatchData==0) {
            for (int j=0; j < int(trigData[i].size()); ++j)
              retId << i << j;
          }//+ Case B: one data item, rest in node
	  else if(segMatchNode==(displaySeg.size()-1) && segMatchData==1) {
            for (int x=0; x < int(dataFlag.size()); x+=2)
              if(dataFlag[x+1]!=0)
                retId << i << dataFlag[x];
          }//+ Case C: at least one in node, 2 or more in data
          else if(segMatchNode!=0 && (segMatchNode + segMatchData)>=displaySeg.size() && segMatchData>1) {
            for (int x=0; x < int(dataFlag.size()); x+=2)
              if(dataFlag[x+1]!=0)
                retId << i << dataFlag[x];
          }//Case D: none in node, all in data
          else if(segMatchNode==0 && segMatchData>=displaySeg.size()) {
            for (int x=0; x < int(dataFlag.size()); x+=2)
              if(dataFlag[x+1]!=0)
                retId << i << dataFlag[x];
          }
        }
	
        //Reset
        dataFlag.clear(); foundSeg.clear();
        segMatchNode=0; segMatchData=0;
      }
      
      if(retId.size()!=0) {
        //Prepare collection for display
        for (int x=0; x < int(retId.size()); x+=2) {
	  
          //Highlight filter segments in results --------------------------------------------------------------->
          hl_trigNode = trigNode[retId[x]];
          hl_trigData = trigData[retId[x]][retId[x+1]];
          for (int k=0; k < int(displaySeg.size()); ++k) {
            //Note: A displaySeg can be part of its prior, which has to be ignored!
            while(hl_b!=-1) {
              hl_b = hl_trigNode.indexOf(displaySeg[k], hl_e, Qt::CaseInsensitive);
              if(hl_b!=-1) {
                hl_e = hl_b + int(displaySeg[k].size());
                if(hlSetNode.size()==0)
                  hlSetNode << hl_b << hl_e;
		else {
                  hlSetNode << hl_b << hl_e;
                  for (int s=0; s < int(hlSetNode.size()); s+=2) {
                    if(hl_b <= hlSetNode[s] && hl_e >= hlSetNode[s+1])
                      ++hl_check;
                    if(hl_b > hlSetNode[s] && hl_e < hlSetNode[s+1])
                      ++hl_check;
                  }
                  if(hl_check==0)
                    hlSetNode << hl_b << hl_e;
                  hl_check=0;
                }
              }
            }
            hl_b=0; hl_e=0;
	    
	    while(hl_b!=-1) {
              hl_b = hl_trigData.indexOf(displaySeg[k], hl_e, Qt::CaseInsensitive);
              if(hl_b!=-1) {
                hl_e = hl_b + int(displaySeg[k].size());
                if(hlSetData.size()==0)
                  hlSetData << hl_b << hl_e;
                else {
                  hlSetData << hl_b << hl_e;
                  for (int s=0; s < int(hlSetData.size()); s+=2) {
                    if(hl_b <= hlSetData[s] && hl_e >= hlSetData[s+1])
                      ++hl_check;
                    if(hl_b > hlSetData[s] && hl_e < hlSetData[s+1])
                      ++hl_check;
                  }
                  if(hl_check==0)
                    hlSetData << hl_b << hl_e;
                  hl_check=0;
                }
              }
            }
            hl_b=0; hl_e=0;
          }

	  //Address: empty space and next-to-next segments
          qSort(hlSetNode); qSort(hlSetData);
          for (int s=0; s < int(hlSetNode.size()); s+=2) {
            //+ next-to-next segments
            if(s>0 && hlSetNode[s]==hlSetNode[s-1]) {
              hlSetNode.removeAt(s-1);
              hlSetNode.removeAt(s-1);//s
            }
            //+ empty space
            if(s>0 && (hlSetNode[s]-hlSetNode[s-1])==1 && hl_trigNode.at(s)==QChar(' ')) {
              hlSetNode.removeAt(s-1);
              hlSetNode.removeAt(s-1);//s
            }
          }
          for (int s=0; s < int(hlSetData.size()); s+=2) {
            //+ next-to-next segments
            if(s>0 && hlSetData[s]==hlSetData[s-1]) {
              hlSetData.removeAt(s-1);
              hlSetData.removeAt(s-1);//s
            }
            //+ empty space
	    if(s>0 && (hlSetNode[s]-hlSetNode[s-1])==1 && hl_trigNode.at(s)==QChar(' ')) {
              hlSetNode.removeAt(s-1);
              hlSetNode.removeAt(s-1);//s
            }
          }
          for (int s=0; s < int(hlSetData.size()); s+=2) {
            //+ next-to-next segments
            if(s>0 && hlSetData[s]==hlSetData[s-1]) {
              hlSetData.removeAt(s-1);
              hlSetData.removeAt(s-1);//s
            }
            //+ empty space
            if(s>0 && (hlSetData[s]-hlSetData[s-1])==1 && hl_trigData.at(s)==QChar(' ')) {
              hlSetData.removeAt(s-1);
              hlSetData.removeAt(s-1);//s
            }
          }
	  
	  //Insert highlighting tags
          for (int s=0; s < int(hlSetNode.size()); s+=2) {
            //retContainer << QString::number(hlSetNode[s]) << QString(" / ") << QString::number(hlSetNode[s+1]) << QString(" | ");
            hl_trigNode.insert(hlSetNode[s]+hl_idx,highlight_b);
            hl_idx += highlight_b.size();
            hl_trigNode.insert(hlSetNode[s+1]+hl_idx,highlight_e);
            hl_idx += highlight_e.size();
          }
          hlSetNode.clear(); hl_idx=0;
          for (int s=0; s < int(hlSetData.size()); s+=2) {
            hl_trigData.insert(hlSetData[s]+hl_idx,highlight_b);
            hl_idx += highlight_b.size();
            hl_trigData.insert(hlSetData[s+1]+hl_idx,highlight_e);
            hl_idx += highlight_e.size();
          }
          hlSetData.clear(); hl_idx=0;
          //-------------------------------------------------------------------------------------------------------<
	  
	  if(newTable) {
            retContainer << table_b + thead_b + hl_trigNode + thead_e; //table head
            retContainer << tbody_b + data_b + hl_trigData + data_e; //first entry
            _idx=retId[x]; newTable=false;
          }
          else if(!newTable && retId[x]==_idx) {
            retContainer << data_b + hl_trigData + data_e;
            if(x==(retId.size()-2)) retContainer << tbody_e + table_e;
          }
          else {
            retContainer << tbody_e + table_e;
            newTable=true; x-=2;
          }
        }
      }
      else
        retContainer << "<b>RESULT ::</b> Sorry, no results found.";
      
      //Append for browser display
      for (int i=0; i < int(retContainer.size()); ++i)
        body.append(retContainer[i]);
    }
    else
      displayResult=false;
    
  }//--> no data
  else body.append("<b>ERROR ::</b> No data to query!");
  
  //Display query results
  if(displayResult)
    printToScreen(body);
  else
    filterDisplayUpdate(">info");
}//END: searchData


//System call: Before next Event loaded: Clear GUI
//_____________________________________________________________________________________________
void VP1TriggerUIChannel::systemErased(IVP1System*)
{
  log_verbose("systemErased()");
  m_treeWidget->setUpdatesEnabled(false);
  m_searchBar->clear();
  m_treeWidget->clear();
  m_treeWidget->setUpdatesEnabled(true);
}//END: systemErased
