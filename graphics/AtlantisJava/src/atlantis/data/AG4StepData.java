package atlantis.data;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.util.Enumeration;
import java.util.Scanner;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import atlantis.parameters.AParameterUtilities;
import atlantis.canvas.ACanvas;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.utils.AAtlantisException;
import atlantis.event.ABufferedEventSource;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.utils.AHashMap;

/**
 * This class holds the data from GEANT4 (G4) steps which
 * represent the path of simulated particles. The steps
 * are read in from a (zipped) text file via the File menu
 * or by pressing "/".
 * The file (named "G4Atlantis_run_event.txt(.zip)") can be
 * made by running AtlasG4 with a special plugin:
 * AtlasG4_trf.py postInitInclude="G4UserActions/G4AtlantisDumper_options.py"
 *
 * @author Andy Haas (ahaas)
 */
public class AG4StepData extends AData {

    protected int[] nsteps;
    protected int[] gid;
    protected int[] pid;
    protected int[] fs;
    protected int[] ls;
    protected int[] stepn;
    protected String[] pn;
    protected int[] nsec;
    protected float[] tedep;
    protected float[] tnedep;
    protected float[] length;
    protected int[] pdg;
    protected float[] mass;
    protected float[] charge;
    protected float[] x1;
    protected float[] y1;
    protected float[] z1;
    protected float[] t1;
    protected float[] x2;
    protected float[] y2;
    protected float[] z2;
    protected float[] t2;
    protected float[] px1;
    protected float[] py1;
    protected float[] pz1;
    protected float[] ke1;
    protected float[] px2;
    protected float[] py2;
    protected float[] pz2;
    protected float[] ke2;

    protected float[] eta1;
    protected float[] p1;

    public AG4StepData(AHashMap p, AEvent e) {
        super(p, e);

        nsteps = p.getIntArray("nsteps");
        gid = p.getIntArray("id");
        pid = p.getIntArray("pid");
        fs = p.getIntArray("fs");
        ls = p.getIntArray("ls");
        stepn = p.getIntArray("stepn");
        pn = p.getStringArray("pn");
        nsec= p.getIntArray("nsec");
        tedep = p.getFloatArray("tedep");
        tnedep = p.getFloatArray("tnedep");
        length = p.getFloatArray("length");
        pdg = p.getIntArray("pdg");
        mass = p.getFloatArray("mass");
        charge = p.getFloatArray("charge");
        x1 = p.getFloatArray("x1");
        y1 = p.getFloatArray("y1");
        z1 = p.getFloatArray("z1");
        t1 = p.getFloatArray("t1");
        x2 = p.getFloatArray("x2");
        y2 = p.getFloatArray("y2");
        z2 = p.getFloatArray("z2");
        t2 = p.getFloatArray("t2");
        px1 = p.getFloatArray("px1");
        py1 = p.getFloatArray("py1");
        pz1 = p.getFloatArray("pz1");
        ke1 = p.getFloatArray("ke1");
        px2 = p.getFloatArray("px2");
        py2 = p.getFloatArray("py2");
        pz2 = p.getFloatArray("pz2");
        ke2 = p.getFloatArray("ke2");

        eta1 = new float[numData];
        p1 = new float[numData];
        for (int i=0; i<numData; ++i) {
            eta1[i]=(float)(-1.0*Math.log(Math.tan(
                    Math.acos(  z1[i]/Math.sqrt(x1[i]*x1[i]+y1[i]*y1[i]+z1[i]*z1[i])  )/2.0
                    )));
            p1[i]=(float)Math.sqrt(px1[i]*px1[i]+py1[i]*py1[i]+pz1[i]*pz1[i]);
        }
    }

    /**
     * The color of the step displayed... mostly determined
     * by the particle type (pdgid) for now.
     */
    @Override
    protected void colorByIndex() {
        int numColors = parameterStore.get("HitColors", "Number").getI();
        if (numColors<6) return;//not enough colors!
        int[] col = parameterStore.getArray("HitColors", "C1", numColors);
        for (int i = 0; i < numData; i++) {

            if (charge[i]==0) color[i] = (byte)col[0];//neutral
            else color[i] = (byte)col[1];//charged

            if (pdg[i]==22) color[i] = (byte)col[2];//photons
            if (Math.abs(pdg[i])==11) color[i] = (byte)col[3];//electrons
            if (Math.abs(pdg[i])==13) color[i] = (byte)col[4];//muons
            if (Math.abs(pdg[i])>=1000000000) color[i] = (byte)col[5];//nuclei
        }
    }

    /**
     * Can choose to color by constant color, or using the
     * function above (by type / index).
     */
    protected int internalColor() {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        switch (colorFunction) {
            case 0:
                colorByConstant();
                break;
            case 1:
                colorByIndex();
                break;
        }
        return 1;
    }

    /**
     * Cuts on the particle type, energy, etc.
     * Adjusted via the gui, and in the config-G4.xml file.
     */
    protected void applyCuts() {
        cutIndex();

        cut("CutsObjects", "G4StepKE1", " ke1", ke1);//starting kinetic energy (MeV)
        cut("CutsObjects", "G4StepP1", " p1", p1);//starting momentum (MeV)
        
        cut("CutsObjects", "G4StepPhotons", " |pdg|", pdg);//22
        cut("CutsObjects", "G4StepElectrons", " |pdg|", pdg);//11
        cut("CutsObjects", "G4StepMuons", " |pdg|", pdg);//13
        cut("CutsObjects", "G4StepPions", " |pdg|", pdg);//211
        cut("CutsObjects", "G4StepProtons", " |pdg|", pdg);//2212
        cut("CutsObjects", "G4StepNeutrons", " |pdg|", pdg);//2112

        // Nuclear codes are given as 10-digit numbers +-10LZZZAAAI.
        // For a nucleus consisting of np protons and nn neutrons
        // A = np + nn +nlambda and Z = np and L = nlambda
        // I gives the isomer level, with I = 0 corresponding to the ground state and I >0 to excitations
        cut("CutsObjects", "G4StepNuclei", " |pdg|", pdg);//1000000000

    }

    /**
     * Prints out stuff when you select a step using the hand.
     */
    public String getHitInfo(int index) {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
        if (simpleOutput > 0) {
            return getNameScreenName() + " index: " + index + "\n";
        }

        String name = "none";
        try {
            name = APDGTable.getName(pdg[index]);

        } catch(APDGTable.ParticleNotFoundError e) {
            name = "unknown";
        }

        return getName() + " (index: " + index + ") : "
                + " storegate key = " + storeGateKey + "\n"
                + " total step # = " + nsteps[index] + " \n"
                + " geant/parent id = " + gid[index]+"/"+ pid[index] + " \n"
                + " first/last step? " + fs[index] +"/"+ ls[index] + " / step#="+stepn[index]+"\n"
                + " process = " + pn[index] + " / # secondaries = " + nsec[index] + " \n"
                + " energy/NI deposited = " + tedep[index]+" / "+tnedep[index]+" MeV\n"
                + " length = " + length[index] + " cm / " +"eta1 = "+eta1[index]+"\n"
                + " pdg = " + pdg[index] + " ("+name+")\n"
                + " mass / charge = " + mass[index]+" MeV"+" / "+charge[index]+"\n"
                + " x1 y1 z1 = " + String.format("%.3f", x1[index])+" "+String.format("%.3f", y1[index])+" "+String.format("%.3f", z1[index])+" cm\n"
                + " t1 = " + String.format("%.3f", t1[index]) + " ns\n"
                + " x2 y2 z2 = " + String.format("%.3f", x2[index])+" "+String.format("%.3f", y2[index])+" "+String.format("%.3f", z2[index])+" cm\n"
                + " t2 = " + String.format("%.3f", t2[index]) + " ns\n"
                + " px1 py1 pz1 = " + String.format("%.3f", px1[index])+" "+String.format("%.3f", py1[index])+" "+String.format("%.3f", pz1[index])+" MeV\n"
                + " ke1 = " + String.format("%.3f", ke1[index]) + " MeV\n"
                + " px2 py2 pz2 = " + String.format("%.3f", px2[index])+" "+String.format("%.3f", py2[index])+" "+String.format("%.3f", pz2[index])+" MeV\n"
                + " ke2 = " + String.format("%.3f", ke2[index]) + " MeV\n"
                + " p1 = " + String.format("%.3f", p1[index]) + " MeV\n"
                ;
    }

    /**
     * Make some cuts for drawing in the YX view.
     */
    @Override
    protected ACoord getYXUser() {
        //System.out.print("numData="+numData+", numDraw="+numDraw);
        makeDrawList();
        //System.out.print(" "+numDraw);

        //cut on |eta1|
        int num = 0;
        double AbsEtaXYcut = parameterStore.get(PARAMETER_GROUP, "AbsEtaXYcut").getD();
        if (AbsEtaXYcut >= 0.0) {
            for (int i = 0; i < numDraw; i++) {
                if (Math.abs(eta1[listdl[i]]) < AbsEtaXYcut) {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
        }
        //System.out.print(" "+numDraw);

        //cut on |z1|
        num = 0;
        double AbszXYcut = parameterStore.get(PARAMETER_GROUP, "AbszXYcut").getD();
        if (AbszXYcut >= 0.0) {
            for (int i = 0; i < numDraw; i++) {
                if (Math.abs(z1[listdl[i]]) < AbszXYcut) {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
        }
        //System.out.println(" "+numDraw);

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        //double length = parameterStore.get(PARAMETER_GROUP, "SegmentLength").getD();
        for (int i = 0; i < numDraw; i++) {
            int j = listdl[i];
            hv[0][i] = new double[]{x1[j], x2[j]};
            hv[1][i] = new double[]{y1[j], y2[j]};
            index[i] = j;
        }
        return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
    }

    /**
     * Make some cuts for drawing in the FR view.
     */
    @Override
    protected ACoord getFRUser() {
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
    }

    /**
     * Make some cuts for drawing in the RZ view.
     */
    @Override
    protected ACoord getRZUser() {
        makeDrawList();

        //cut on |eta1|
        int num = 0;
        double AbsEtaRZcut = parameterStore.get(PARAMETER_GROUP, "AbsEtaRZcut").getD();
        if (AbsEtaRZcut >= 0.0) {
            for (int i = 0; i < numDraw; i++) {
                if (Math.abs(eta1[listdl[i]]) < AbsEtaRZcut) {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
        }
        //cut on |z1|
        num = 0;
        double AbszRZcut = parameterStore.get(PARAMETER_GROUP, "AbszRZcut").getD();
        if (AbszRZcut >= 0.0) {
            for (int i = 0; i < numDraw; i++) {
                if (
                        Math.abs(z1[listdl[i]]) < AbszRZcut &&
                        Math.sqrt(x1[listdl[i]]*x1[listdl[i]]+y1[listdl[i]]*y1[listdl[i]]) < 1200
                        ) {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
        }

        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];
        //double length = parameterStore.get(PARAMETER_GROUP, "SegmentLength").getD();
        for (int i = 0; i < numDraw; i++) {
            int j = listdl[i];
            double s = AParameterUtilities.getRhoSign(x1[j], y1[j]);//use same for both points to avoid ugliness
            double r1 = s * Math.sqrt(x1[j] * x1[j] + y1[j] * y1[j]);
            double r2 = s * Math.sqrt(x2[j] * x2[j] + y2[j] * y2[j]);
            hv[0][i] = new double[]{z1[j], z2[j]};
            hv[1][i] = new double[]{r1, r2};
            index[i] = j;
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }

    @Override
    public String getName() {
        return "G4Step";
    }

    @Override
    public String getNameScreenName() {
        return "G4Step";
    }

    @Override
    public String getParameterGroup() {
        return "G4Step";
    }

    /**
     * Read in the corresponding G4Atlantis_run_event.txt(.zip) file.
     * It needs to be in the same directory as the JiveXML_run_event.xml file
     * that is currently being viewed.
     */
    public static void ReadG4Steps(){
        AEvent ev = eventManager.getCurrentEvent();
                        String source = eventManager.getEventSource().getSourceName();
                        System.out.println("Source="+source);
                        String filename = source;
                        if (source.contains("\\")) filename+="\\G4Atlantis_"+ev.getRunNumber()+"_"+ev.getEventNumber()+".txt";
                        else filename += "/G4Atlantis_" + ev.getRunNumber() + "_" + ev.getEventNumber() + ".txt";
                        System.out.println("Reading G4 file: " + filename);
                        Scanner inFile = null;
                        int maxnstepsinthisevent = 4000000;//max allowed
                        int nstepsinfile = maxnstepsinthisevent;//will be set to actual number of steps in file, if lower than maximum
                        try {
                            inFile = new Scanner(new FileReader(filename));

                            //try to get the number of steps that are in the file
                            try {
                                FileInputStream in = new FileInputStream(filename);
                                BufferedReader br = new BufferedReader(new InputStreamReader(in));
                                String strLine = null, tmp;
                                while ((tmp = br.readLine()) != null) { strLine = tmp; }
                                String lastLine = strLine;
                                System.out.println(lastLine);
                                in.close();

                                Scanner lastlinescanner = new Scanner(lastLine);
                                String llach = lastlinescanner.next();
                                String _llach = "ACH_G4Step";
                                if (!llach.equals(_llach)) {
                                    System.out.println("??ach in lastline = " + llach);
                                }
                                nstepsinfile = lastlinescanner.nextInt()+1;

                            } catch (IOException ex) {
                                Logger.getLogger(AG4StepData.class.getName()).log(Level.SEVERE, null, ex);
                            }

                        } catch (FileNotFoundException ex) {
                            //Logger.getLogger(AEventQueue.class.getName()).log(Level.SEVERE, null, ex);
                            System.out.println("Could not open "+filename);
                        }

                        if (inFile == null) {
                            ZipFile zipfile = null;
                            try {
                                System.out.println("Reading G4 file: " + filename+".zip");
                                zipfile = new ZipFile(filename + ".zip");
                            } catch (IOException ex) {
                                //Logger.getLogger(AEventQueue.class.getName()).log(Level.SEVERE, null, ex);
                                System.out.println("Could not open "+filename+".zip either");
                            }
                            if (zipfile != null) {
                                BufferedInputStream is = null;
                                ZipEntry entry;
                                Enumeration e = zipfile.entries();
                                while (e.hasMoreElements()) {
                                    entry = (ZipEntry) e.nextElement();
                                    System.out.println("Extracting: " + entry);
                                    try {
                                        is = new BufferedInputStream(zipfile.getInputStream(entry));
                                        BufferedReader br = new BufferedReader(new InputStreamReader(is));
                                        String strLine = null, tmp;
                                        while ((tmp = br.readLine()) != null) {
                                            strLine = tmp;
                                        }
                                        String lastLine = strLine;
                                        System.out.println(lastLine);

                                        Scanner lastlinescanner = new Scanner(lastLine);
                                        String llach = lastlinescanner.next();
                                        String _llach = "ACH_G4Step";
                                        if (!llach.equals(_llach)) {
                                            System.out.println("??ach in lastline = " + llach);
                                        }
                                        nstepsinfile = lastlinescanner.nextInt() + 1;

                                        is = new BufferedInputStream(zipfile.getInputStream(entry));
                                        inFile = new Scanner(is);
                                    } catch (IOException ex) {
                                        Logger.getLogger(AG4StepData.class.getName()).log(Level.SEVERE, null, ex);
                                    }

                                }
                            }
                        }

                        if (inFile != null) {

                            System.out.print("nsteps in file should be "+nstepsinfile+"... ");
                            if (nstepsinfile>maxnstepsinthisevent) nstepsinfile=maxnstepsinthisevent;
                            System.out.println(" and will read "+nstepsinfile+" steps.");

                            try {
                            AHashMap para = new AHashMap(100);
                            float[] x1 = new float[nstepsinfile];
                            float[] y1 = new float[nstepsinfile];
                            float[] z1 = new float[nstepsinfile];
                            float[] t1 = new float[nstepsinfile];
                            float[] x2 = new float[nstepsinfile];
                            float[] y2 = new float[nstepsinfile];
                            float[] z2 = new float[nstepsinfile];
                            float[] t2 = new float[nstepsinfile];
                            float[] px1 = new float[nstepsinfile];
                            float[] py1 = new float[nstepsinfile];
                            float[] pz1 = new float[nstepsinfile];
                            float[] ke1 = new float[nstepsinfile];
                            float[] px2 = new float[nstepsinfile];
                            float[] py2 = new float[nstepsinfile];
                            float[] pz2 = new float[nstepsinfile];
                            float[] ke2 = new float[nstepsinfile];
                            int[] nsteps = new int[nstepsinfile];
                            int[] id = new int[nstepsinfile];
                            int[] pid = new int[nstepsinfile];
                            int[] fs = new int[nstepsinfile];
                            int[] ls = new int[nstepsinfile];
                            int[] stepn = new int[nstepsinfile];
                            String[] pn = new String[nstepsinfile];
                            int[] nsec = new int[nstepsinfile];
                            float[] tedep = new float[nstepsinfile];
                            float[] tnedep = new float[nstepsinfile];
                            float[] length = new float[nstepsinfile];
                            int[] pdg = new int[nstepsinfile];
                            float[] mass = new float[nstepsinfile];
                            float[] charge = new float[nstepsinfile];

                            int n = 0;
                            String _ach = "ACH_G4Step";
                            while (inFile.hasNext() && n<nstepsinfile){
                            String ach = inFile.next();
                            if (!ach.equals(_ach)) {
                                System.out.println("??ach = " + ach);
                            }
                            nsteps[n] = inFile.nextInt();
                            id[n] = inFile.nextInt();
                            pid[n] = inFile.nextInt();
                            fs[n] = inFile.nextInt();
                            ls[n] = inFile.nextInt();
                            stepn[n] = inFile.nextInt();
                            pn[n] = inFile.next();
                            nsec[n] = inFile.nextInt();
                            tedep[n] = inFile.nextFloat();
                            tnedep[n] = inFile.nextFloat();
                            length[n] = inFile.nextFloat()/10.0f;
                            pdg[n] = inFile.nextInt();
                            //System.out.println("pdg="+pdg);
                            mass[n] = inFile.nextFloat();
                            charge[n] = inFile.nextFloat();
                            x1[n] = inFile.nextFloat()/10.0f;
                            y1[n] = inFile.nextFloat()/10.0f;
                            z1[n] = inFile.nextFloat()/10.0f;
                            t1[n] = inFile.nextFloat();
                            px1[n] = inFile.nextFloat();
                            py1[n] = inFile.nextFloat();
                            pz1[n] = inFile.nextFloat();
                            ke1[n] = inFile.nextFloat();
                            x2[n] = inFile.nextFloat()/10.0f;
                            y2[n] = inFile.nextFloat()/10.0f;
                            z2[n] = inFile.nextFloat()/10.0f;
                            t2[n] = inFile.nextFloat();
                            px2[n] = inFile.nextFloat();
                            py2[n] = inFile.nextFloat();
                            pz2[n] = inFile.nextFloat();
                            ke2[n] = inFile.nextFloat();
                            ++n;
                            if (n%100000==0) System.out.println("Read "+n+" steps");
                            }
                            System.out.println("Read "+n+" steps");
                            inFile.close();
                            System.out.println("Closed file.");

                            para.put("nsteps",nsteps);
                            para.put("id",id);
                            para.put("pid",pid);
                            para.put("fs",fs);
                            para.put("ls",ls);
                            para.put("stepn",stepn);
                            para.put("pn",pn);
                            para.put("nsec",nsec);
                            para.put("tedep",tedep);
                            para.put("tnedep",tnedep);
                            para.put("length",length);
                            para.put("pdg",pdg);
                            para.put("mass",mass);
                            para.put("charge",charge);
                            para.put("x1", x1);
                            para.put("y1", y1);
                            para.put("z1", z1);
                            para.put("t1", t1);
                            para.put("x2", x2);
                            para.put("y2", y2);
                            para.put("z2", z2);
                            para.put("t2", t2);
                            para.put("px1", px1);
                            para.put("py1", py1);
                            para.put("pz1", pz1);
                            para.put("ke1", ke1);
                            para.put("px2", px2);
                            para.put("py2", py2);
                            para.put("pz2", pz2);
                            para.put("ke2", ke2);
                            para.put("numData", n);
                            //para.put("storeGateKey", "G4");

                            ABufferedEventSource.maxNumberOfEvents=1;//otherwise we run out of heap
                            AG4StepData g4d = new AG4StepData(para, ev);
                            ev.add(g4d);
                            } catch (AAtlantisException ex) {
                                Logger.getLogger(AG4StepData.class.getName()).log(Level.SEVERE, null, ex);
                            }
                            catch (OutOfMemoryError oom) {
                                Logger.getLogger(AG4StepData.class.getName()).log(Level.SEVERE,"Ran out of memory while trying to read G4Step data");
                            }
                            ACanvas.getCanvas().repaintAllFromScratch();
                        }//inFile!=null

    }

}
