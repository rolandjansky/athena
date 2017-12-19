
/************************************************/
/*                                              */
/*              EXPERT CORNER                   */
/*                                              */
/*                                              */
/*  (If you don't want to change the way that)  */
/*  (messages are displayed, rather you just )  */
/*  (want to edit messages, then skip down to)  */
/*  (the USER CORNER                         )  */
/*                                              */
/************************************************/

var messages = new Array();

/******************************************/
/* A simple Javascript class for messages */
/******************************************/

function Message(date,text,isImportant){
   this.theDate     = date;
   this.theText     = text;
   this.isImportant = isImportant;
   messages.push(this);
}

/***************************************************/
/* Functions to do with displaying on the web page */
/***************************************************/

/* Display style for the message text */
function textStyle(isImportant){
   style = 'font-family:arial;font-size:10pt;';
   if (isImportant){
       style = style + 'font-weight:bold;color:red';
   }
   else{
       style = style + 'color:black';
   }

   return style;
}

/* Display style for the date*/
function dateStyle(){
   return 'font-family:arial;font-size:10pt;color:black;font-weight:bold';
}

function titleStyle(){
   return 'font-family:sans-serif;font-weight:bold;font-size:10pt;color:#88f;background-color:#ccf';
}

/* HTML code for displaying a message*/
function display(numbMessagesToDisplay,title){
   htmlCode  = '<div align="center">';
   htmlCode += '<table width="100%" border="0" cellspacing="0" cellpadding="0" bgcolor="#dddddd">';
   htmlCode += '<tr><td colspan= "4" align="center" style="' + titleStyle() + '">' + title + '</td></tr>'

   for (var i=messages.length-1;i >= (messages.length - numbMessagesToDisplay);i--){
      message = messages[i];
      htmlCode += '<tr style="' + dateStyle() + '">';
      htmlCode += '<td>' + message.theDate + '</td>';
      htmlCode += '</tr>';
      htmlCode += '<tr style="' + textStyle(message.isImportant) + '">';
      htmlCode += '<td>' + message.theText + '</td>';
      htmlCode += '</tr>';
   }

   htmlCode += '</table>';
   htmlCode += '</div>';

   document.write(htmlCode);

}

/************************************************/
/*                                              */
/*                USER CORNER                   */
/*                                              */
/*                                              */
/************************************************/

new Message('27th February 2007','We currently run SLC3 and SLC4 (32 bit) AFS dev nightlies. Things to note:<ul><li>rel_2: SLC4_32 platform available for checking</li><li>rel_3: SLC3 platform available for checking</li><li>rel_4: RTT run, but low quality build.</li><li>rel_0/1/4/5/6: both platforms available for checking</li></ul>',false);

new Message('1st March 2007','Today\'s rel_4 runs have completed successfully. Problems identified with yesterday\'s SLC4 rel_3 run:<ul><li>Run manually shutdown: 8 InDetRTT jobs affected. Cause: Batch system did not return these jobs.</li></ul>',false);

new Message('2nd March 2007','The JOBOPTIONSSEARCHPATH env var has changed format in today\'s rel_5 compared to yesterday. The RTT runs have been delayed while we adapted our code. SLC4 has now completed. SLC3 ongoing.',false);

new Message('3rd March 2007','Today\'s rel_6 SLC4 run complete. SLC3 run starts at 13:00. Yesterday\'s SLC3 run did not shut down correctly, but all packages completed.',false);

new Message('5th March 2007','Problems with SLC3 runs since Friday. Investigation ongoing.',false);

new Message('6th March 2007','Yesterday\'s rel_1 SLC3 build has been re-run and is complete. Will re-run rel_0 SLC3 shortly. 12.0.6 run seems to have been killed (probably by the network problems) before completion. Is re-running now.<br /><b>Note: RTT pilot jobs have been switched off for the past few days. Hence total number of jobs has dropped considerably.</b>',false);

new Message('7th March 2007','rel_0 has been re-run. 12.0.6 is essentially done (there are a couple of jobs stuck on the batch system apparently). Today\'s rel_3 SLC4 run has almost completed.',false);

new Message('8th March 2007','Today\'s rel_4 SLC4 had problems due to planned IT network intervention. Re-running. No SLC3 build was available yesterday. None will be available today (SLC3 build machines down).',false);

new Message('9th March 2007','Today\'s rel_5 SLC4 is running. No SLC3 run available yet. None for past 2 days.',false);

new Message('10th March 2007','Today\'s rel_6 SLC4 has run except for one long queue job. No SLC3 run available yet.',false);

new Message('12th March 2007','Today\'s rel_1 SLC4 has run. No SLC3 run available yet.',false);

new Message('13th March 2007','Today\'s rel_2 SLC4 has run. SLC3 build is running.',false);

new Message('14th March 2007','Today\'s SLC4 run accidentally terminated. Re-running.',false);

new Message('16th March 2007','Today\'s SLC4 rel_5 run ongoing.',false);

new Message('18th March 2007','Problems with these web pages being addressed.',true);

new Message('19th March 2007','Web pages accessible again. Previous tag back into production for now, rel_1 SLC4 ongoing.',false);

new Message('20th March 2007','rel_2 SLC4 done. SLC3 ongoing.',false);

new Message('21st March 2007','This morning\'s SLC4 run has just started. The current tag remains in production for now.',false);

new Message('22nd March 2007','This morning\'s rel_4 SLC4 run is complete. No SLC3 build yet available.',false);

new Message('25th March 2007','This morning\'s rel_0 SLC4 run is proceeding very slowly.',false);

new Message('26th March 2007','This morning\'s rel_1 dev SLC4 run postponed to allow running of val SLC4 nightly.',false);

new Message('27th March 2007','This morning\'s rel_2 val SLC4 run ongoing. SLC4 dev rel_2 starts 13:00.',false);

new Message('2nd April 2007','This morning\'s rel_1 val SLC4 run ongoing. SLC4 dev rel_1 starts 13:00.',false);

new Message('5th April 2007','This morning\'s rel_3 SLC4 bugfix run complete (44/243 jobs OK). rel_3 val starts 12:00; rel_3 SLC3 bugfix at 18:00.',false);

new Message('6th April 2007','This morning\'s rel_5 SLC4 bugfix AFS build is not yet available.',false);

new Message('9th April 2007','There was no rel_0 SLC3 bugfix build available yesterday. SLC4 was available, and was run (as was the val nightly)',false);

new Message('9th April 2007','Batch queues have been inactivated (by an administrator, not us). Consequently no RTT runs are making progress.',true);

new Message('10th April 2007','Batch queues appear normal again. This problem resulted in:<ul><li>Yesterday\'s rel_1 bugfix SLC3 build not run</li><li>Majority of jobs in rel_1 val/bugfix SLC4 build timed out.</li></ul>',true);

new Message('11th April 2007','rel_3 SLC4 bugfix done. val build ongoing (not yet web visible).',false);

new Message('12th April 2007','rel_4 SLC4 bugfix done. val build ongoing.',false);

new Message('13th April 2007','rel_5 SLC4 bugfix and val runs ongoing.',false);

new Message('14th April 2007','rel_6 SLC4 bugfix is complete, but should be ignored. The NICOS-produced flag that the RTT team was told could be used as a reliable indication that the release was ready appears, today, to have been set before all projects were truly available. The RTT thus processed an incomplete build.', true);

new Message('17th April 2007','The machine on which RTT production runs are launched was switched off by IT a couple of days ago due to problems. It is still off, and so we have switched to the only other machine available to us. Unfortunately that machine appears sluggish so we have not yet been able to launch an RTT run.',false);

new Message('17th April 2007','We are currently running yesterday\'s rel_1 SLC4 bugfix.',false);

new Message('18th April 2007','Our main RTT production machine remains down whilst IT investigate. A second production machine to which we switched was rebooted last night whilst we were running yesterday\'s rel_2 SLC4 bugfix. The run was of course killed. We are currently re-running it but it will be stopped shortly to allow us to put in place RunTimeTester-00-01-85. When in place, we\'ll start up today\'s rel_3 val build.',false);

new Message('18th April 2007','The ongoing run of yesterday\'s rel_2 SLC4 bugfix build has been halted. Today\'s rel_3 val build launches at 18:30 (using new tag RunTimeTester-00-01-85).',false);

new Message('18th April 2007','Today\'s rel_3 val is almost done. SLC3 bugfix ongoing.',false);

new Message('19th April 2007','Yesterday\'s ongoing rel_3 SLC3 bugfix and val runs were killed for reasons not understood. Today\'s rel_4 SLC4 bugfix build is done and available. rel_4 val is ongoing. If available, rel_4 SLC3 bugfix will launch tonight.',false);

new Message('20th April 2007','<ul><li>Today\'s rel_5 SLC4 bugfix run ongoing.</li><li>Yesterday\'s rel_4 val run closed down with loss of 6 jobs (G4AtlasApps/JetRec packages). Why? Machine was under heavy stress and SLC3 bugfix run was starting up. 2 ongoing runs would have probably crashed the machine.</li></ul>',false);

new Message('20th April 2007','Today\'s rel_5 SLC4 bugfix is done. val run launches at 14:20.',false);

new Message('20th April 2007','Today\'s rel_5 SLC4 bugfix is done. val run finished, but is being launched again: batch system failed to return ~70 job logs.',false);

new Message('20th April 2007','No rel_5 SLC3 bugfix build today.',false);

new Message('23rd April 2007','Yesterday\'s bugfix and today\'s SLC4 bugfix runs should be ignored: AFS build was bad. Edit: This morning\'s SLC4 bugfix has been fixed, and so will be re-run.',false);

new Message('23rd April 2007','Over the weekend RTT runs were ongoing, but failed to appear on the web pages. The pages will be manually updated shortly to make visible these results. <b>Edit: DONE. rel_1 SLC4 bugfix ongoing.</b>', false);

new Message('24th April 2007','No rel_1 val build available yesterday. No rel_2 builds available so far today.',false);

new Message('25th April 2007','No rel_2 bugfix SLC3 build available yesterday. Today\'s rel_3 bugfix almost done. val nightly to be re-run following fix to the build.',false);

new Message('26th April 2007','Today\'s rel_4 val run ongoing. No bugfix builds available yet.', false);

new Message('27th April 2007','No builds available yet.', false);

new Message('30th April 2007','Since Friday the flag set by NICOS and used by the RTT to indicate that a given build is ready, has not been set. The RTT did not run any builds, except for yesterday\'s rel_0 val, for which the flag was set manually by Alex (thanks!).', true);

new Message('30th April 2007','Today\'s rel_1 SLC4 bugfix almost done. val nightly ongoing.', false);

new Message('2nd May 2007','Today\'s rel_3 val nightly is launching soon. No bugfix builds available yet.', false);

new Message('4th May 2007','Today\'s rel_5 val ongoing; but build looks corrupt.', false);

new Message('5th May 2007','Today\'s rel_6 val run launches at 11:00.', false);

new Message('5th May 2007','Today\'s rel_6 val run ongoing.', false);

new Message('7th May 2007','Disk quota exceeded over the weekend. Both rel_6 bugfix runs affected: 2-3 jobs (CaloRecEx/egammaRec) not completed.', false);

new Message('8th May 2007','Today\'s rel_2 SLC4 bugfix and val runs are ongoing.', false);

new Message('8th May 2007','All today\'s rel_2 runs complete (in all runs, some jobs from CaloRecEx/egammaRec got stuck in batch and were aborted manually).', false);

new Message('13th May 2007','There was no rel_6 val build available yesterday.', false);

new Message('16th May 2007','The empty "Project" column below (if you are on the start page with the list of available RTT runs) will gradually be filled.', false);

new Message('7th June 2007','<h2>Here is the only working link to <a target="main_frame" href="http://cern.ch/atlasrtt/13.0.10/build/i686-slc4-gcc34-opt/offline/page2.xml">13.0.10 results</a>.</h2>', true);

new Message('18th June 2007','<h2>There are problems with RTT cluster. Because of these, there were no runs yesterday 17th and none this morning.Please check RTT hypernews forum for updates.</h2>', true);

new Message('20th June 2007','<h3>RTT running has resumed.</h3>', false);

new Message('21st June 2007','<ul><li><h2>Here is the only working link to <a target="main_frame" href="http://cern.ch/atlasrtt/13.0.10/build/i686-slc4-gcc34-opt/offline/page2.xml">13.0.10 results</a>.</h2></li></ul>', true);

new Message('6th June 2007','<ul><li><h2>Here are the only working links to <a href="http://cern.ch/atlasrtt/page1.php?xml=13.0.10/build/i686-slc4-gcc34-opt/offline/RTTSummary.xml&viewBy=package">13.0.10 results</a> and <a href="http://cern.ch/atlasrtt/page1.php?xml=13.0.20/build/i686-slc4-gcc34-opt/offline/RTTSummary.xml&viewBy=package">13.0.20 results</a>.</h2></li></ul>', true);

/***********************************************************/
/*        ADDING A NEW MESSAGE                             */
/***********************************************************/
/* Add your latest message above this comment box          */
/* Skeleton code is:                                       */
/*   new Message('theDate','theMessage',false);            */
/*                                                         */
/*  where false means the message gets displayed normal    */
/*        true means message displayed in red and bold     */
/***********************************************************/

/* Show the last how many messages ? */
display(numberOfMessagesToDisplay = 1,title=":: Message of the Day ::");
