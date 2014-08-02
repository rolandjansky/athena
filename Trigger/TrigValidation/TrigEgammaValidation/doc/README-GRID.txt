* How to run the TrigEgammaValidation package using pathena

To use TrigEgammaValidation through the Grid, 
checkout and compile the package on the desired 
version and copy runGrid.py, CKolmogorov.C, 
CompareFilesKolmogorov.C, ProcessValidation.C, 
ReFinalize.C and generalJobOption.py to a test directory.

Also, checkout and compile the latest release of 
PhysicsAnalysis/DistributedAnalysis/PandaTools for the
pathena and psequencer tools.

On your test directory, setup runGrid.py script on
the area indicated on the file. You probably only 
have to change your username and email address information
(this configuration will
be moved to another file for clarity on a next release).

Now you are all setup:

$ mkdir output
$ python runGrid.py

This generates seq_*.txt files, which contain the sequence
of commands to execute. For each one of them, open a
screen session and run, on that directory:

$ psequencer seq_?.txt

(where seq_?.txt is one of the seq files)

This will run pathena. After it's done, you will receive an email.
psequencer will wait for this email and proceed when there 
are no failures in the job (or after the 5th try).
The post-processing will follow automatically and all information
will be available at the output directory.

Copy the output directory to your Results directory on lxplus
to show the results on the web, through the php pages.

Last change:
* Danilo Enoque Ferreira de Lima (21/01/2009)

