/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1ChannelManager              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

/////////////////////////////////////// CHANNELS ///////////////////////////////////////////////////////
//
//  NB: It is assumed that all channel methods are very fast to execute. Real work belongs in systems!!
//
//  Constructor: Allocate systems, and call registerSystem on
//               them. After the constructor the name(), information() and type()
//               methods should work (and their returned values should not change
//               throughout the channel lifetime. You may never delete a system yourself.
//
//  Create: You can assume that all systems have had their create()
//          method already called. Here you can perform any initialisation you
//          need to, including allocating a controller if you need to and
//          registering it with registerController. You may never delete a controller yourself.
//
//  The next two methods, refresh and erase, will be called an
//  undetermined amount of times by the framework (including zero), and
//  always in the cycle: refresh-erase-refresh-erase-refresh-erase-...-refresh-erase
//
//  Refresh: You can assume that all systems have just had their refresh
//           methods called, and thus have just processed data from a new event. Any gui
//           updates you need to perform (e.g. changing the zoom level of a viewer to
//           encompass all the items put into it by the systems) you can do here.
//
//  Erase: You can assume that all systems have just had their erase methods called.
//         The goal here is to stop the channel from displaying any
//         event data, since we are preparing to leave an event and load
//         a new one. If we did not blank out the presented data here,
//         we might end up in a situation where different channels
//         displays data from different events at the same time! (For
//         accumulators this is of course a little different).
//
//  Uncreate: You can assume that all systems have had their uncreate()
//            method already called. Here you can perform any release of resources you
//            need to (remember to no delete systems or the controller!).
//            Dont emit any signals during this method (or during the destructor).
//
//  Destructor: There really should not be anything left to do here...
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1ChannelManager.h"
#include "VP1Gui/VP1ExecutionScheduler.h"
#include "VP1Gui/VP1MainWindow.h"

#include "VP1Base/IVP1ChannelWidgetFactory.h"
#include "VP1Base/IVP1ChannelWidget.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QMap>
#include <QScrollArea>

#include <map>
#include <set>
#include <cassert>

//QT_NO_PLUGIN_CHECK

class VP1ChannelManager::Imp {

public:

	VP1ChannelManager*channelmanager;
	VP1ExecutionScheduler*scheduler;
	VP1MainWindow*mainwindow;

	//Actual created channels:
	std::map<QString,std::set<IVP1ChannelWidget*> > basename_2_channels;
	std::map<QString,IVP1ChannelWidget*> uniquename_2_channel;

	//plugin info:
	std::map<QString,std::pair<QStringList,IVP1ChannelWidgetFactory *> > pluginfile_2_basenamesAndFactory;
	std::map<QString,QString> basename_2_pluginfile;
	std::map<QString,QPluginLoader*> pluginfile_2_pluginloader;

	QList<QObject*> additionalOwnedObjects;//For deletion in destructor
	void emit_basename_availableChannelListChanged();
	void emit_basename_availableUnusedChannelListChanged();

	std::map<IVP1ChannelWidget*,QWidget*> channel_2_controller;
	QMap<QString,QString> basename2iconlocation;

	//These next four should always be used to properly new, create,
	//uncreate and delete channels and their systems.  The channel is
	//responsible for new'ing systems in its constructor, but
	//create/uncreate/delete of systems are handled by the
	//channelmanager. Controllers are also deleted by the channelmanager.
	IVP1ChannelWidget * constructChannel( QString channelbasename, QString& err, bool init = true );

	QString channelTypeToIconLocation(const IVP1ChannelWidget::Type& type) const;
};

//___________________________________________________________________________________
VP1ChannelManager::VP1ChannelManager(VP1ExecutionScheduler*sched,VP1MainWindow*mw) : d(new Imp)
{
	d->channelmanager=this;
	d->scheduler=sched;
	d->mainwindow=mw;
}

//___________________________________________________________________________________
VP1ChannelManager::~VP1ChannelManager()
{
	while(!d->uniquename_2_channel.empty())
		deleteChannel(d->uniquename_2_channel.begin()->first);

	std::map<QString,QPluginLoader*>::const_iterator it, itE = d->pluginfile_2_pluginloader.end();
	for(it=d->pluginfile_2_pluginloader.begin();it!=itE;++it)
		delete it->second;

	foreach (QObject* o,d->additionalOwnedObjects)
	delete o;
	delete d;
}

//___________________________________________________________________________________
//Fixme: Return error string in case of problems.
bool VP1ChannelManager::deleteChannel(QString channeluniquename) {

	//Find info:
	assert(d->uniquename_2_channel.find(channeluniquename)!=d->uniquename_2_channel.end());
	IVP1ChannelWidget * cw = d->uniquename_2_channel[channeluniquename];
	QString bn = cw->name();
	assert(d->basename_2_channels.find(bn)!=d->basename_2_channels.end());

	//Remove channelwidget from maps, and delete it.
	assert(d->basename_2_channels[bn].find(cw)!=d->basename_2_channels[bn].end());
	d->basename_2_channels[bn].erase(d->basename_2_channels[bn].find(cw));
	d->uniquename_2_channel.erase(d->uniquename_2_channel.find(channeluniquename));

	d->scheduler->uncreateAndDelete(cw);//The actual uncreate()+delete might be postponed if a system is
	//presently refreshing. But for all bookkeeping purposes it already
	//happened!

	channelUncreated(cw);

	//Remove controller from maps - and ensure the widget gets deleted
	//in any case (it is a scrollarea, not the controller known by the
	//actual channelwidget):
	assert(d->channel_2_controller.find(cw)!=d->channel_2_controller.end());
	d->channel_2_controller[cw]->deleteLater();
	d->channel_2_controller.erase(d->channel_2_controller.find(cw));
	assert(d->channel_2_controller.find(cw)==d->channel_2_controller.end());

	//Finally, assign new cloneids to any remaining channels of the same basename:
	//First wipe old info in uniquename_2_channel.
	std::set<IVP1ChannelWidget*>::iterator it = d->basename_2_channels[bn].begin();
	std::set<IVP1ChannelWidget*>::iterator itE = d->basename_2_channels[bn].end();
	for (;it!=itE;++it) {
		assert(d->uniquename_2_channel.find((*it)->unique_name())!=d->uniquename_2_channel.end());
		d->uniquename_2_channel.erase(d->uniquename_2_channel.find((*it)->unique_name()));
	}

	it = d->basename_2_channels[bn].begin();
	unsigned id(0);
	for (;it!=itE;++it) {
		(*it)->setCloneID(id++);
		(*it)->unique_name();
		assert(d->uniquename_2_channel.find((*it)->unique_name())==d->uniquename_2_channel.end());
		d->uniquename_2_channel[(*it)->unique_name()]=(*it);
	}

	uniquename_channelListChanged(uniquenames());
	d->emit_basename_availableUnusedChannelListChanged();
	return true;
}


//___________________________________________________________________________________
bool VP1ChannelManager::unloadPluginFile(QString filename) {

	VP1Msg::messageVerbose("VP1ChannelManager::unloadPluginFile()");

	if (d->pluginfile_2_pluginloader.find(filename)==d->pluginfile_2_pluginloader.end())
		return false;

	assert(d->pluginfile_2_basenamesAndFactory.find(filename)!=d->pluginfile_2_basenamesAndFactory.end());
	assert(d->pluginfile_2_pluginloader.find(filename)!=d->pluginfile_2_pluginloader.end());

	//Find basenames provided by this plugin, as well as factory and pluginloader:
	QStringList bns = channelsInPluginFile(filename);
	IVP1ChannelWidgetFactory * fact = d->pluginfile_2_basenamesAndFactory.find(filename)->second.second;
	QPluginLoader* loader = d->pluginfile_2_pluginloader[filename];

	//Abort if any active channels are affected by this plugin:
	foreach (QString bn, bns)
	if (nActive(bn)>0)
		return false;

	//Delete factory:
	delete fact;

	//Unload:
	bool success = loader->unload();
	delete loader;

	//Update maps:
	d->pluginfile_2_basenamesAndFactory.erase(d->pluginfile_2_basenamesAndFactory.find(filename));
	d->pluginfile_2_pluginloader.erase(d->pluginfile_2_pluginloader.find(filename));
	foreach (QString bn, bns) {
		d->basename_2_channels.erase(d->basename_2_channels.find(bn));
		d->basename_2_pluginfile.erase(d->basename_2_pluginfile.find(bn));
	}

	//Emit signals regarding changes:
	uniquename_channelListChanged(uniquenames());
	d->emit_basename_availableChannelListChanged();
	d->emit_basename_availableUnusedChannelListChanged();

	//Return:
	return success;
}

//___________________________________________________________________________________
QString VP1ChannelManager::loadPluginFile(QString filename)
{

	VP1Msg::messageVerbose("VP1ChannelManager::loadPluginFile() - filename: " + filename);

	QString bn = QFileInfo(filename).fileName();

	if (d->pluginfile_2_basenamesAndFactory.find(filename)!=d->pluginfile_2_basenamesAndFactory.end()) {
		return "Error: Plugin already loaded.";
	} else {


		QFileInfo fi(filename);
		if (!fi.exists())
			return "File does not exist";
		if (!fi.isReadable())
			return "File is not readable";

		VP1Msg::messageVerbose("BB");

		VP1Msg::messageVerbose("plugin path: " + fi.absoluteFilePath());
		VP1Msg::messageVerbose("app path: " + QCoreApplication::applicationDirPath());
		VP1Msg::messageVerbose("library path: " + QLibraryInfo::location(QLibraryInfo::PluginsPath));

		QString pluginAbsPath = fi.absoluteFilePath();

		QPluginLoader * loader = new QPluginLoader(pluginAbsPath);//Fixme: Ensure all loaders gets unloaded on shutdown (and thus deleted)
		d->pluginfile_2_pluginloader[filename]=loader;

		bool loadOk = loader->load();

		if (!loadOk) {
			VP1Msg::message("ERROR!!! plugin load failed!!! trying to load the 'dbg' build...");

			/*
			 * when running VP1 sometimes (especially from the devval release) the
			 * load of the plugin fails silently...
			 * After a long investigation, I found that the problem is in the mismatch
			 * between the compilation flag of the main Qt application
			 * and the flag of the plugin modules.
			 * For example the main application is compiled in 'dbg', while the plugins
			 * are compiled in 'opt' mode...
			 * We still understand why...
			 * In the meantime I implemented a workaround: the 'opt' flag is replaced
			 * by the 'dbg' flag when the plugin fails to load.
			 *
			 */
			// QString build = "x86_64-slc6-gcc47-opt";
			//QString buildB = "x86_64-slc6-gcc47-dbg";
			//pluginAbsPath.replace(build, buildB);

			pluginAbsPath.replace("opt", "dbg");

			loader = new QPluginLoader(pluginAbsPath);
			loadOk = loader->load();

			if (loadOk) VP1Msg::message("Ok! 'dbg' build of the plugin successfully loaded!");
			else VP1Msg::message("ERROR!! Also the 'dbg' build failed to load!!");

		}


		QObject *plugin = loader->instance();


		if (!plugin) {

			bool error = loader->errorString().isNull();


			QString errStr = "ERROR!!";
			if (error)
				errStr = QString("Did not find any compatible plugins in file");
			else
				errStr = loader->errorString();


			return errStr;
		}


		IVP1ChannelWidgetFactory * fact = qobject_cast<IVP1ChannelWidgetFactory *>(plugin);
		d->additionalOwnedObjects << plugin;


		if (!fact)
			return "Did not find any channel factory in plugin file";


		QStringList providedbasenames = fact->channelWidgetsProvided();


		if (providedbasenames.count()==0)
			return "Found channel factory in plugin file, but no advertised channels!";


		d->pluginfile_2_basenamesAndFactory[filename] = std::pair<QStringList,IVP1ChannelWidgetFactory *>(providedbasenames,fact);


		foreach (QString bn,providedbasenames) {
			if (d->basename_2_pluginfile.find(bn)!=d->basename_2_pluginfile.end())
				return "Channels navp1 '"+bn+"' are already provided by plugin file "
						+d->basename_2_pluginfile[bn]+" (ignoring other plugins in file '"+filename+"')";
		}


		foreach (QString bn,providedbasenames) {
			d->basename_2_pluginfile[bn] = filename;
			d->basename_2_channels[bn] = std::set<IVP1ChannelWidget*>();
		}


		d->emit_basename_availableChannelListChanged();
		d->emit_basename_availableUnusedChannelListChanged();


		//Fixme: check 20 chars.
	}


	d->mainwindow->addToMessageBox( "Successfully loaded "+bn, "color:#008b00" );
	QString out = "providing channels: "+channelsInPluginFile(filename).join(", ");
	d->mainwindow->addToMessageBox( out, "color:#008b00" );


	return "";
}

//___________________________________________________________________________________
QStringList VP1ChannelManager::channelsInPluginFile(QString filename) const {
	if (d->pluginfile_2_basenamesAndFactory.find(filename)==d->pluginfile_2_basenamesAndFactory.end())
		return QStringList();
	return d->pluginfile_2_basenamesAndFactory[filename].first;
}

//___________________________________________________________________________________
IVP1ChannelWidget * VP1ChannelManager::Imp::constructChannel( QString channelbasename, QString& err, bool init ) {
	if (basename_2_pluginfile.find(channelbasename)==basename_2_pluginfile.end()) {
		err = "Did not find plugin providing channel named '"+channelbasename+"'";
		return 0;
	}
	QString plf = basename_2_pluginfile[channelbasename];
	//sanity:
	assert(pluginfile_2_basenamesAndFactory.find(plf)!=pluginfile_2_basenamesAndFactory.end()&&"This should never happen!");
	assert(pluginfile_2_basenamesAndFactory[plf].first.contains(channelbasename)&&"This should never happen!");
	IVP1ChannelWidget * cw = pluginfile_2_basenamesAndFactory[plf].second->getChannelWidget(channelbasename);
	if (!cw) {
		err = "Plugin file '"+plf+"' did not provide channel navp1 '"+channelbasename+"' despite advertising this capability!";
		return 0;
	} else {
		err="";
	}

	if (channelbasename!=cw->name()) {
		err = "Actual basename of channel widget constructed by factory is not the one requested: '"+cw->name()
    				  +"' vs. '"+channelbasename+"'. This is most likely due to a mistake in the plugin!!";
		delete cw;
		return 0;
	}

	//Cache the icon associated with this channel for future use (and override if there is already a cache):
	QString icontext = channelTypeToIconLocation(cw->type());
	basename2iconlocation[cw->name()]=icontext;

	if (init)
		cw->init();

	//Sanity: should not have controller registered:
	assert(channel_2_controller.find(cw)==channel_2_controller.end());

	return cw;
}

//___________________________________________________________________________________
IVP1ChannelWidget * VP1ChannelManager::getChannel( QString channelbasename, QString& err ) {
	IVP1ChannelWidget * cw = d->constructChannel( channelbasename, err );
	if (!cw)
		return 0;
	cw->setUpdatesEnabled(false);
	cw->setCloneID(nActive(channelbasename));
	assert(d->uniquename_2_channel.find(cw->unique_name())==d->uniquename_2_channel.end());
	d->uniquename_2_channel[cw->unique_name()]=cw;
	d->basename_2_channels[channelbasename].insert(cw);

	assert(cw->state()==IVP1ChannelWidget::CONSTRUCTED);
	d->scheduler->bringFromConstructedToReady(cw);//Fixme: should return bool
	assert(cw->state()==IVP1ChannelWidget::READY);

	assert(d->channel_2_controller.find(cw)==d->channel_2_controller.end());
	QWidget * controller = cw->controllerWidget();
	QScrollArea * scrollarea = new QScrollArea;
	//  controller->setParent(scrollarea);
	scrollarea->setObjectName("VP1ControllerScrollArea");
	scrollarea->setWidgetResizable(true);
	scrollarea->setFrameShape(QFrame::NoFrame);
	scrollarea->setFrameShadow(QFrame::Plain);


	//scrollArea->setBackgroundRole(QPalette::Dark);
	scrollarea->setWidget(controller);
	d->channel_2_controller[cw] = scrollarea;

	uniquename_channelListChanged(uniquenames());
	d->emit_basename_availableUnusedChannelListChanged();
	newChannelCreated(cw);
	cw->setUpdatesEnabled(true);

	return cw;
}

//___________________________________________________________________________________
QWidget* VP1ChannelManager::getController(IVP1ChannelWidget*cw) {
	assert(d->channel_2_controller.find(cw)!=d->channel_2_controller.end());
	return d->channel_2_controller[cw];
}

//___________________________________________________________________________________
unsigned VP1ChannelManager::nActive( QString channelbasename ) const {
	if (d->basename_2_channels.find(channelbasename)==d->basename_2_channels.end())
		return 0;
	return d->basename_2_channels[channelbasename].size();
}

//fixme: Always check uniqueness (and <20chars) of basenames!

//___________________________________________________________________________________
IVP1ChannelWidget* VP1ChannelManager::uniqueName2Channel(QString uniquename) const {
	if (d->uniquename_2_channel.find(uniquename)==d->uniquename_2_channel.end())
		return 0;
	return d->uniquename_2_channel[uniquename];
}

//___________________________________________________________________________________
bool VP1ChannelManager::uniqueNameExists(QString uniquename) const {
	return (d->uniquename_2_channel.find(uniquename)!=d->uniquename_2_channel.end());
}

//___________________________________________________________________________________
bool VP1ChannelManager::baseNameExists(QString basename) const {
	return (d->basename_2_pluginfile.find(basename)!=d->basename_2_pluginfile.end());
}

//___________________________________________________________________________________
QStringList VP1ChannelManager::uniquenames() const {
	QStringList l;
	std::map<QString,IVP1ChannelWidget*>::iterator it = d->uniquename_2_channel.begin();
	std::map<QString,IVP1ChannelWidget*>::iterator itE = d->uniquename_2_channel.end();

	for (;it!=itE;++it)
		l << it->first;

	return l;
}


//___________________________________________________________________________________
void VP1ChannelManager::Imp::emit_basename_availableChannelListChanged() {
	QStringList l;

	std::map<QString,QString>::const_iterator it = basename_2_pluginfile.begin();
	std::map<QString,QString>::const_iterator itE = basename_2_pluginfile.end();
	for (;it!=itE;++it) {
		l << it->first;
	}
	channelmanager->availableChannelListChanged(l);

}

//___________________________________________________________________________________
QStringList VP1ChannelManager::availableChannelList() {
	QStringList l;
	std::map<QString,QString>::const_iterator it = d->basename_2_pluginfile.begin();
	std::map<QString,QString>::const_iterator itE = d->basename_2_pluginfile.end();
	for (;it!=itE;++it) {
		l << it->first;
	}
	return l;
}

//___________________________________________________________________________________
void VP1ChannelManager::Imp::emit_basename_availableUnusedChannelListChanged() {
	QStringList l;

	std::map<QString,QString>::const_iterator it = basename_2_pluginfile.begin();
	std::map<QString,QString>::const_iterator itE = basename_2_pluginfile.end();
	for (;it!=itE;++it) {
		if (channelmanager->nActive(it->first)==0)
			l << it->first;
	}

	channelmanager->availableUnusedChannelListChanged(l);
}

//___________________________________________________________________________________
QStringList VP1ChannelManager::serializePluginInfo() const {
	std::map<QString,std::pair<QStringList,IVP1ChannelWidgetFactory *> >::const_iterator
	it, itE = d->pluginfile_2_basenamesAndFactory.end();

	QStringList l;
	for( it = d->pluginfile_2_basenamesAndFactory.begin(); it!=itE; ++it ) {
		l<<QFileInfo(it->first).fileName();
	}
	return l;
}

//___________________________________________________________________________________
QStringList VP1ChannelManager::currentPluginFiles() const {
	std::map<QString,QPluginLoader*>::const_iterator it = d->pluginfile_2_pluginloader.begin();
	std::map<QString,QPluginLoader*>::const_iterator itE = d->pluginfile_2_pluginloader.end();
	QStringList l;
	for(;it!=itE;++it) {
		l<<it->first;
	}
	return l;
}

//___________________________________________________________________________________
QStringList VP1ChannelManager::basename2UniqueNames(QString basename) const {

	if (d->basename_2_channels.find(basename)==d->basename_2_channels.end())
		return QStringList();

	std::set<IVP1ChannelWidget*>::const_iterator it = d->basename_2_channels[basename].begin();
	std::set<IVP1ChannelWidget*>::const_iterator itE = d->basename_2_channels[basename].end();

	QStringList l;
	for (;it!=itE;++it) {
		l << (*it)->unique_name();
	}
	return l;
}

//___________________________________________________________________________________
bool VP1ChannelManager::channelWithBasenameIsLoaded(QString basename) const {
	if (d->basename_2_channels.find(basename)==d->basename_2_channels.end())
		return false;
	return !d->basename_2_channels[basename].empty();
}


//___________________________________________________________________________________
QString VP1ChannelManager::Imp::channelTypeToIconLocation(const IVP1ChannelWidget::Type& type) const
{
	if (type==IVP1ChannelWidget::THREEDIMENSIONAL) return ":/vp1/icons/icons/3d_32x32.png";
	if (type==IVP1ChannelWidget::TWODIMENSIONAL) return ":/vp1/icons/icons/kivio_32x32.png";//Fixme: Better icon.
	if (type==IVP1ChannelWidget::NUMBERS) return ":/vp1/icons/icons/kcalc_32x32.png";
	if (type==IVP1ChannelWidget::HISTOGRAM) return ":/vp1/icons/icons/kchart_32x32.png";
	if (type==IVP1ChannelWidget::UNKNOWN) return ":/vp1/icons/icons/help_32x32.png";
	return "";
}

//___________________________________________________________________________________
bool VP1ChannelManager::hasRefreshingSystem(const QString& uniquename) const
{
	IVP1ChannelWidget*cw = uniqueName2Channel(uniquename);
	return cw ? cw->hasRefreshingSystem() : false;
}

//___________________________________________________________________________________
QString VP1ChannelManager::getIconLocation(const QString& channelname, const bool& isbasename)
{
	if (!isbasename) {
		IVP1ChannelWidget* cw = uniqueName2Channel(channelname);
		if (!cw)
			return "";
		return d->channelTypeToIconLocation(cw->type());
	}

	if (d->basename2iconlocation.contains(channelname))
		return d->basename2iconlocation[channelname];

	//We have to briefly construct a channel in order to get its type.
	//The following create and destruct maneouver is to get a proper
	//cache of the icontext associated with the channel:
	QString err;
	IVP1ChannelWidget * cw = d->constructChannel( channelname, err, false /*don't init()*/ );
	if (!cw)
		return "";
	cw->deleteControllers();
	delete cw;
	assert(d->channel_2_controller.find(cw)==d->channel_2_controller.end());

	//Try again;
	if (d->basename2iconlocation.contains(channelname))
		return d->basename2iconlocation[channelname];
	return "";
}
