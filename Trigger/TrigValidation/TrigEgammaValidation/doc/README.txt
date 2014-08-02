# Danilo Enoque Ferreira de Lima and Denis Oliveira Damazio
# Danilo.Enoque.Ferreira.de.Lima@cern.ch
# Denis.Oliveira.Damazio@cern.ch
# date : 03/16/2007

In order to see a very simple web page, just type the following
commands :
cp -R Summary-dev ${MY_HTTP_PATH_IN_AFS}
# create dummy RTTResults path
mkdir ${MY_HTTP_PATH_IN_AFS}/RTTResults/

Normally, MY_HTTP_PATH_IN_AFS is ${HOME}/www.
You should be able to see a simple page at :
http://www.cern.ch/${USERNAME}/Summary-dev/

Playing with content.xml and having actual results from RTT in the
above defined path will allow you to show your histograms.
