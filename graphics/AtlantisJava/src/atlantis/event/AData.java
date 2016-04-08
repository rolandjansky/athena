package atlantis.event;

import java.awt.Component;
import java.util.Vector;
import java.util.Collection;
import javax.swing.Action;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.canvas.AWindow;
import atlantis.data.ACompositeParticleData;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.ADrawable;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.list.AListManager;
import atlantis.output.AExceptionHandler;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjection3D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.utils.AIntHashtable;
import atlantis.utils.AMath;
import atlantis.list.AListProcessor;
import atlantis.utils.ALogger;

/**
 * Base class which holds the data from a subdetector (e.g. TRT) This
 * representation is appropriate for data with a static number of entries but
 * not for example for reconstructed vertices the number of which may be changed
 * interactively by the user. AData should perhaps become an interface.
 */
abstract public class AData implements ADrawable, AListProcessor {
  private static final ALogger logger = ALogger.getLogger(AData.class);  //the event that this data belongs to
  protected static AEventManager eventManager = AEventManager.instance();
  protected static APar parameterStore = APar.instance();
  protected static Component guiComponent = AGlobals.instance().getGuiFrame();
  protected final AEvent event;
  protected static final int NO_INVERSE = -1;
  protected int numData;
  protected int numDraw;
  protected int[] listdl;
  protected byte[] color;
  protected int[] id;
  protected AIntHashtable indexFromId;
  protected String storeGateKey = null;
  protected final String PARAMETER_GROUP = getParameterGroup();
  
  abstract protected int internalColor();

  abstract protected void applyCuts();

  abstract public String getNameScreenName();

  abstract public String getHitInfo(int index); // pick info

  abstract public String getParameterGroup();
  // Holds the projection object that we are currently drawing on. This is *NOT* the same
  // as ACanvas.getCanvas().getCurrentWindow().getProjection(), since this function gives
  // the projection in the active window. Particularly at startup windows will be redrawn
  // without being the active window. Hence this variable.
  //
  // By the way: this is also inherently bad design. Why does the data need 
  // to know about the projection? The main flaw is that the data draws itself,
  // rather then the graphics context drawing the data - S.B.
  protected AProjection2D currentProjection;

  // dummy version, only applies to dynamic data sources (e.g. RecVertex (RVx))
  public void remove(int[] index) {}

  // used to finalize the state of this object
  // after all other AData objects are essentially built
  // e.g. calculate the number of S3D hits on a track after both are read
  protected void finalizeConstruction() {}

  // x y and z are now stored
  // rho and phi arrays are only correct after a call to this function
  protected void calculateRhoPhi() {}

  // used by rubberband in V-Plot
  public String getVPHitInfo() {
    return "";
  }

  public AData(AHashMap p, AEvent e) {
    //save reference to the event this data belongs to
    event = e;

    numData = p.getInt("numData");
    numDraw = numData;
    listdl = new int[numData];
    color = new byte[numData];

    storeGateKey = (String) p.get("storeGateKey");
    if ("".equals(storeGateKey) || storeGateKey == null) {
      storeGateKey = null;
    }

    if (p.get("id") != null) {
      id = p.getIntArray("id");
    } else {
      id = new int[numData];
      for (int i = 0; i < numData; i++) {
        // start IDs calculated in Atlantis from 0, should be the same
        // as in JiveXML
        id[i] = i;
      }
    }

    indexFromId = new AIntHashtable(id);
  } // AData()

  AData(Node node, AEvent e) {
    //save reference to the event this data belongs to
    event = e;
    numData = 0;
    NodeList children = node.getChildNodes();
    for (int i = 0; i < children.getLength(); i++) {
      if (children.item(i).getNodeType() == Node.ELEMENT_NODE) {
        numData++;
      }
    }
    numDraw = numData;
    listdl = new int[numData];
    color = new byte[numData];
    id = new int[numData];
    for (int i = 0; i < numData; i++) {
      id[i] = i;
    }
    indexFromId = new AIntHashtable(id);
  }

  public AEvent getEvent(){
    return event;
  }

  public int getIndexFromId(int id) {
    return indexFromId.get(id);
  }

  public final A4Vector get4Vector() {
    makeDrawList();
    return get4Vector(numDraw, listdl);
  }

  // only applies to data which can provide 4 vectors
 public A4Vector get4Vector(int num, int[] list) {
    return null;
  }
 
 // only applies to data which can provide 4 vectors
 public A4Vector get4Vector(int num, int[] list, double mass) {
    return null;
  }

  public int getNumData() {
    return numData;
  }

  public int getNumDraw() {
    return numDraw;
  }

  public int getDrawIndex(int index) {
    return this.listdl[index];
  }

  public int[] getID() {
    return id;
  }

  protected int getNum(String name, int index) {
    int[][] ass = event.getAssociationManager().get(getFullName(), name);
    if (ass != null && ass[index] != null) {
      return ass[index].length;
    } else {
      return 0;
    }
  }

  protected int[] getNum(String name) {
    int[][] ass = event.getAssociationManager().get(getFullName(), name);
    int[] num = new int[numData];
    if (ass != null) {
      for (int i = 0; i < ass.length; ++i) {
        if (ass[i] != null) {
          num[i] = ass[i].length;
        }
      }
    }
    return num;
  }

  public byte[] getColor() {
    color();
    byte[] temp = new byte[id.length];

    for (int i = 0; i < temp.length; i++) {
      temp[i] = color[getIndexFromId(id[i])];
    }
    return temp;
  }

  public int[] getColor(int[] dl) {
    color();
    int[] temp = new int[dl.length];

    for (int i = 0; i < temp.length; i++) {
      temp[i] = color[dl[i]];
    }
    return temp;
  }
  
  public int getColor(int index){
      return color[index];
  }

  public void makeDrawList() {
    calculateRhoPhi();
    constructDefaultDrawlist();
    applyCuts();
  }

  public void constructDefaultDrawlist() {
    if (!parameterStore.get("CutsATLAS", "ByList").getStatus()) {
      // if some hits are on a list they must be drawn last
      int[][] temp = AListManager.getInstance().getColorMapping(this);
      int[] index = temp[0];
      int[] ctemp = temp[1];
      if (index.length > 0) {
        // AOutput.logln("AData.constructDefaultDrawlist() numData = " +
        // numData);
        int[] c = new int[numData];
        final int NONE = -999;
        for (int i = 0; i < numData; ++i) {
          c[i] = NONE;
        }
        for (int i = 0; i < index.length; ++i) {
          c[index[i]] = ctemp[i];
        }
        numDraw = 0;
        for (int i = 0; i < numData; ++i) {
          if (c[i] == NONE) {
            listdl[numDraw++] = i;
          }
        }
        for (int i = 0; i < numData; ++i) {
          if (c[i] != NONE && c[i] != AColorMap.INVISIBLE) {
            listdl[numDraw++] = i;
          // nb invisible are not in the drawlist
          }
        }
      } else {
        // this is the standard case
        numDraw = numData;
        for (int i = 0; i < numDraw; ++i) {
          listdl[i] = i;
        }
      }
    } else {
      // drawlist set by cuts.....
      boolean[] selected = AListManager.getInstance().getSelection(this);
      numDraw = 0;
      for (int i = 0; i < selected.length; ++i) {
        if (selected[i]) {
          listdl[numDraw++] = i;
        }
      }
    }
  }

  // provides the draw list as a boolean array parallel to the data array
  protected boolean[] isDrawn() {
    boolean[] drawn = new boolean[numData];

    for (int i = 0; i < numDraw; i++) {
      drawn[listdl[i]] = true;
    }
    return drawn;
  }

  public void cut(String groupName, String parameterName, String text, float[] array) {
    AParameter par = parameterStore.get(groupName, parameterName);

    if (par != null && par.getStatus() && array != null) {
      double value = par.getD();
      String operator = par.getOperator();
      int num = 0;

      if (par.isModulus()) {
        if (operator.equals("<")) {
          for (int i = 0; i < numDraw; i++) {
            if (Math.abs(array[listdl[i]]) < value) {
              listdl[num++] = listdl[i];
            }
          }
        } else if (operator.equals(">")) {
          // '>' really means '>=' for reals since they are real in
          // from ascii file and don't have full precison
          for (int i = 0; i < numDraw; i++) {
            if (Math.abs(array[listdl[i]]) >= value) {
              listdl[num++] = listdl[i];
            }
          }
        } else {
          throw new Error(operator + " operator not sensible for floats");
        }
      } else {
        if (operator.equals("<")) {
          for (int i = 0; i < numDraw; i++) {
            if (array[listdl[i]] < value) {
              listdl[num++] = listdl[i];
            }
          }
        } else if (operator.equals(">")) {
          // '>' really means '>=' for reals since they are real in
          // from ascii file
          // and don't have full precison
          for (int i = 0; i < numDraw; i++) {
            if (array[listdl[i]] >= value) {
              listdl[num++] = listdl[i];
            }
          }
        } else {
          throw new Error(operator + " operator not sensible for floats");
        }
      }
      numDraw = num;
    }
  }

  protected void cut(String groupName, String parameterName, String text, int[] array) {
    AParameter par = parameterStore.get(groupName, parameterName);

    if (par != null && par.getStatus() && array != null) {
      int value = par.getI();

      // consider two special cases: Electron/isEM and Photon/isEM
      if (parameterName.endsWith("isEM")) {
        if (!isEMValidate(value)) {
          return;
        }
        String binaryString = Integer.toBinaryString(value);
        Vector<String> possibleValues = getPossibleValues(binaryString);
        cutIsEM(text, array, possibleValues);
      } else {
        String operator = par.getOperator();
        boolean modulus = par.isModulus();

        cut(text, modulus, array, operator, value);
      }
    }
  }

  protected void cut(String text, int[] array, String operator, int value) {
    cut(text, false, array, operator, value);
  }

  protected void cut(String text, boolean modulus, int[] array, String operator, int value) {
    int num = 0;

    if (modulus) {
      if (operator.equals("<")) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) < value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">")) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) > value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("=") || operator.equals("==")) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) == value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("!=") || operator.equals(AMath.NOTEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) != value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("<=") || operator.equals(AMath.LESSEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) <= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">=") || operator.equals(AMath.GREATEREQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (Math.abs(array[listdl[i]]) >= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else {
        throw new Error(operator + " operator not sensible for integers");
      }
    } else {
      if (operator.equals("<")) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] < value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">")) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] > value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("=") || operator.equals("==")) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] == value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("!=") || operator.equals(AMath.NOTEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] != value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("<=") || operator.equals(AMath.LESSEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] <= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">=") || operator.equals(AMath.GREATEREQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (array[listdl[i]] >= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else {
        throw new Error(operator + " operator not sensible for integers");
      }
    }
    numDraw = num;
  }

  // cut based on the i'th item in the data array
  public void cutIndex() {
    AParameter par = parameterStore.get("CutsATLAS", "Index");

    if (par.getStatus()) {
      int value = par.getI();
      String operator = par.getOperator();
      int num = 0;

      if (operator.equals("<")) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] < value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">")) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] > value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("=") || operator.equals("==")) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] == value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("!=") || operator.equals(AMath.NOTEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] != value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals("<=") || operator.equals(AMath.LESSEQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] <= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else if (operator.equals(">=") || operator.equals(AMath.GREATEREQUAL)) {
        for (int i = 0; i < numDraw; i++) {
          if (listdl[i] >= value) {
            listdl[num++] = listdl[i];
          }
        }
      } else {
        throw new Error(operator + " operator not sensible for integers");
      }
      numDraw = num;
    }
  }

  protected void cutArray(int[] value, int cutValue, String description) {
    int num = 0;

    for (int i = 0; i < numDraw; i++) {

      if (value[listdl[i]] == cutValue) {
        listdl[num++] = listdl[i];
      }
    }
    numDraw = num;
  }

 protected void cutArray(byte[] value, int cutValue, String description) {
    int num = 0;

    for (int i = 0; i < numDraw; i++) {

      if (value[listdl[i]] == cutValue) {
        listdl[num++] = listdl[i];
      }
    }
    numDraw = num;
  }

  //isEMString cut declaration
  //
  // Clone of above cut, but to allow values less than or equal to the cut value
  // At the moment, Specific to isEMString for Electrons and Photons, and isTauString.
  // 
  protected void cutArrayEnum(Vector<Enum> value, int cutValue, String description) {
	    int num = 0;

	    for (int i = 0; i < numDraw; i++) {
	      if ((value.get(listdl[i]).ordinal()) <= cutValue) {
	        listdl[num++] = listdl[i];
	      }
	    }
	    numDraw = num;
	  }
  // End isEMString cut declaration
  
  protected void cutArray(boolean[] value, boolean cutValue, String description) {
    int num = 0;

    for (int i = 0; i < numDraw; i++) {
      if (value[listdl[i]] == cutValue) {
        listdl[num++] = listdl[i];
      }
    }
    numDraw = num;
  }
  
  protected void cutArrayOR(int[] value, int cutValue1, int cutValue2,
          String description) {
    int num = 0;

    for (int i = 0; i < numDraw; i++) {
      if (value[listdl[i]] == cutValue1 || value[listdl[i]] == cutValue2) {
        listdl[num++] = listdl[i];
      }
    }
    numDraw = num;
  }

  // apply phi+-deltaphi cut to lines (e.g. TRT)
  protected void cutPhi(float[] phi, float[] dphi) {
    AParameter par = parameterStore.get("CutsATLAS", "CutPhi");

    if (!par.getStatus()) {
      return;
    }
    double phiCut = Math.toRadians(par.getD());
    double phiMid = Math.toRadians(AMath.nearestPhiDegrees(parameterStore.get("CutsATLAS", "PhiMiddle").getD()));
    double phiLowerCut = phiMid - phiCut;
    double phiUpperCut = phiMid + phiCut;
    int num = 0;

    for (int i = 0; i < numDraw; i++) {
      int list = listdl[i];
      double phiHit = AMath.nearestPhiRadians(phi[list], phiMid);
      double phiLower = phiHit - dphi[list];
      double phiUpper = phiHit + dphi[list];

      if (phiUpper - phiLowerCut > 1.0e-6 && phiUpperCut - phiLower > 1.0e-6) {
        listdl[num++] = list;
      }
    }
    numDraw = num;
  }
  
  /** Apply phi+-deltaphi cut to points (e.g. S3D) */
  public void cutPhi(float[] phi) {
    AParameter par = parameterStore.get("CutsATLAS", "CutPhi");
    boolean usePhiCut = par.getStatus();

    if (usePhiCut) {
      double phiCut = Math.toRadians(Math.abs(par.getD()));
      double phiMid = Math.toRadians(AMath.nearestPhiDegrees(parameterStore.get("CutsATLAS", "PhiMiddle").getD()));
      int num = 0;

      for (int i = 0; i < numDraw; i++) {
        double phiDiff = Math.abs(AMath.nearestPhiRadians(phi[listdl[i]], phiMid) - phiMid);

        if (phiCut - phiDiff > 1.0e-6) {
          listdl[num++] = listdl[i];
        }
      }
      numDraw = num;
    }

  }

  /** apply eta+-deltaeta cut to points (e.g. S3D) */
  public void cutEta(float[] eta) {
    AParameter par = parameterStore.get("CutsATLAS", "CutEta");
    boolean useEtaCut = par.getStatus();

    if (useEtaCut) {
      double etaCut = Math.abs(par.getD());
      double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
      int num = 0;

      for (int i = 0; i < numDraw; i++) {
        double etaDiff = Math.abs(eta[listdl[i]] - etaMid);

        if (etaCut - etaDiff > 1.0e-6) {
          listdl[num++] = listdl[i];
        }
      }
      numDraw = num;
    }
  }

  protected void colorByConstant() {
    if (!PARAMETER_GROUP.equals("CompositeParticle")) {
      int constantColor = parameterStore.get(PARAMETER_GROUP, "Constant").getI();

      for (int i = 0; i < numData; i++) {
        color[i] = (byte) constantColor;
      }
    } else {
      ACompositeParticleData compositeParticles = (ACompositeParticleData) this;
      compositeParticles.makeDrawList();
      for (int e = 0; e < compositeParticles.getNumDraw(); ++e) {
        int list = compositeParticles.getDrawIndex(e);
        switch (Math.abs(compositeParticles.getPdgId(list))) {
          case 13:
            color[list] = (byte) parameterStore.get("Muon", "Constant").getI();
            break;
          case 11:
            color[list] = (byte) parameterStore.get("Electron", "Constant").getI();
            break;
          case 22:
            color[list] = (byte) parameterStore.get("Photon", "Constant").getI();
            break;
          case 5:
            color[list] = (byte) parameterStore.get("BJet", "Constant").getI();
            break;
          case 15:
            color[list] = (byte) parameterStore.get("TauJet", "Constant").getI();
            break;
          case 24:
              color[list] = (byte) parameterStore.get("CompositeParticle", "ConstantW").getI();
              break;
          case 6:
              color[list] = (byte) parameterStore.get("CompositeParticle", "ConstantTop").getI();
              break;
          case 25:
              color[list] = (byte) parameterStore.get("CompositeParticle", "ConstantH").getI();
              break;
                        
          default:
            if (compositeParticles.getTypeEV(list).equals("EVParticleJet")) {
              color[list] = (byte) parameterStore.get("Jet", "Constant").getI();
            } else {
              color[list] = (byte) parameterStore.get("CompositeParticle", "Constant").getI();
            }
        }
      }
    }
  }


  /** Colour by index.
   * e.g. colorByIndex(indexBy(phi));
   * will allow nearby phi tracks to be coloured differently.
   * @param index
   */
  protected void colorByIndex(int[] index) {
    int numColors = parameterStore.get("HitColors", "Number").getI();

    numColors = Math.min(7, numColors);
    int[] col = parameterStore.getArray("HitColors", "C1", numColors);
    for (int i = 0; i < numData; i++) {
      color[index[i]] = (byte) col[i % numColors];
    }
  }

  protected void colorByIndex() {
    int numColors = parameterStore.get("HitColors", "Number").getI();

    numColors = Math.min(7, numColors);
    int[] col = parameterStore.getArray("HitColors", "C1", numColors);
    for (int i = 0; i < numData; i++) {
      color[i] = (byte) col[i % numColors];
    }
  }

  protected void colorByCollection() {
    int constantColor = parameterStore.get(PARAMETER_GROUP, "Constant").getI();
    int numColors = parameterStore.get("HitColors", "Number").getI();
    int[] col = parameterStore.getArray("HitColors", "C1", numColors);
    AEvent ev = eventManager.getCurrentEvent();
    // vector with store gate keys for a particular datatype name: this.getName()
    Vector<String> v = ev.getCollections().get(this.getName());
    int keyIndex = 0;
    if (v != null && (keyIndex = v.indexOf(this.getStoreGateKey())) != -1) {
      // v is null - requested datatype name doesn't exists in the current
      // event - 
      // if keyIndex is -1 it means the current
      // store gate key doesn't exists in the current event (for a datatype
      // which exists in the current event)
      // previous event(s) are being drawn and this colouring function
      // won't work for them but only for the current event

      // constant colour index is used purely as modifier so as all datatypes
      // coloured by colletictions don't start at the same colour ...
      keyIndex += constantColor;
      for (int i = 0; i < numData; i++) {
        color[i] = (byte) col[keyIndex % numColors];
      }
    } else {
      logger.debug("colorByCollection(): can't colour by collection");
    }

  } // colorByCollection() ------------------------------------------------

  /**
   * Colour another class by association. This class is providing the colour,
   * we assign colours from the local class to the target using col[] and id[][].
   * 
   * @param col target class colour array
   * @param id association array from this class to the target class
   * @param unconnectedColor colour for unconnected hits
   * @param sharedColor  colour for shared hits
   */
  protected void colorByAssociation(AData target, int[][] id,
          byte unconnectedColor, byte sharedColor) {
    
      // All hits are unconnected by default
      for (int i=0; i<target.color.length; i++) {
          target.color[i] = unconnectedColor;
      }
      
      if (id == null) return;

      // Loop over drawn objects in this class, then colour the other elements
      // that are associated. i iterates over the drawn items, j is the true 
      // index and k refers to an associated item in the target class, with true 
      // index l.
      for (int i=0; i<numDraw; i++) {
          int j = listdl[i];
          
          if (id[j] != null) {
              for (int k = 0; k < id[j].length; k++) {
                  int l = target.getIndexFromId(id[j][k]);
                  
                  if (l < 0) continue;

                  if (target.color[l] != unconnectedColor) {
                      // If item l was already connected, that means it is shared
                      target.color[l] = sharedColor;
                  } else {
                      // Assign associated item l the colour of item j in the local class
                      target.color[l] = this.color[j];
                  }
              }
          }
      }
  } // colorByAssociation() -----------------------------------------------

  protected void setColor(int index, byte color) {
    if (index >= 0 && index < this.color.length) {
      this.color[index] = color;
    } else {
      logger.debug(getClass().getName() + ":  Set color for data with index = " + index);
    }
  }

  protected void setColor(byte color) {
    for (int i = 0; i < this.color.length; i++) {
      this.color[i] = color;
    }
  }

  protected void colorBy(int[] coloringVariable) {
    int numColors = parameterStore.get("HitColors", "Number").getI();

    numColors = Math.min(7, numColors);
    int[] col = parameterStore.getArray("HitColors", "C1", numColors);

    for (int i = 0; i < numData; i++) {
      color[i] = (byte) col[coloringVariable[i] % numColors];
    }
  }

  protected void colorBy(String colorGroup, int[] coloringVariable) {
    int numColors = parameterStore.get(colorGroup, "Number").getI();

    numColors = Math.min(7, numColors);
    int[] col = parameterStore.getArray(colorGroup, "C1", numColors);

    for (int i = 0; i < numData; i++) {
      color[i] = (byte) col[coloringVariable[i] % numColors];
    }
  }

  protected void colorBy(float[] Pt) {
    int numColors = parameterStore.get("HitColors", "Number").getI();

    numColors = Math.min(7, numColors);
    int[] col = parameterStore.getArray("HitColors", "C1", numColors);
    for (int i = 0; i < numData; i++) {
      int icol = 0;
      if (Math.abs(Pt[i]) < 2) {
        icol = 4; //lichtblauw
      } else if (Math.abs(Pt[i]) < 4) {
        icol = 2; //blauw
      } else if (Math.abs(Pt[i]) < 10) {
        icol = 5; //paars
      } else if (Math.abs(Pt[i]) < 40) {
        icol = 3; //oranje
      } else {
        icol = 0; //rood
      }
      color[i] = (byte) col[icol];
    }
  }

  public String getName() {
    return getParameterGroup();
  }

  public String getStoreGateKey() {
    return storeGateKey;
  }

  // x y from rho phi
  protected void calculateXY(float[] rho, float[] phi, float[] x, float[] y) {
    for (int i = 0; i < x.length; ++i) {
      x[i] = (float) (rho[i] * Math.cos(phi[i]));
      y[i] = (float) (rho[i] * Math.sin(phi[i]));
    }
  }
  
  /** Calculate rho phi from x y */
  protected void calculateRhoPhi(float[] x, float[] y, float[] rho, float[] phi) {
    // [2007-07-20] method is again always called from AData.makeDrawList()
    // but reference to primary vertex is removed, it's not taken into account now
    // double[] pVtx = Atlantis.getEventManager().getCurrentEvent().getPrimaryVertex();
    // double dx = x[i] - pVtx[0];
    // double dy = y[i] - pVtx[1];

    for (int i = 0; i < rho.length; i++) {
      double dx = x[i];
      double dy = y[i];

      rho[i] = (float) (Math.sqrt(dx * dx + dy * dy));
      phi[i] = (float) (Math.atan2(dy, dx));
      if (phi[i] < 0.) {
        phi[i] += AMath.TWO_PI;
      }
    }
  }

  /** Draw the data form this detector on this window and projection */
  public void draw(AWindow window, AGraphics ag, AProjection2D projection) {
    ag.draw(window.calculateDisplay(getUser(projection)));
  }

  /** Get the data from a projection without non linear transform applied.
   * @param projection
   * @return
   */
  protected ACoord getUserNoTransform(AProjection projection) {
    ACoord data = ACoord.NO_DATA;
    // don't use reflection as debugging is then harder
    if (projection instanceof AProjectionYX) {
      data = getYXUser();
    } else if (projection instanceof AProjectionFR) {
      data = getFRUser();
    } else if (projection instanceof AProjectionRZ) {
      data = getRZUser();
    } else if (projection instanceof AProjectionXZ) {
      data = getXZUser();
    } else if (projection instanceof AProjectionYZ) {
      data = getYZUser();
    } else if (projection instanceof AProjectionFZ) {
      data = getFZUser();
    } else if (projection instanceof AProjectionVP) {
      data = getVPUser();
    } else if (projection instanceof AProjection3D) {
      data = get3DUser();
    }

    return data;
  }

  /** Get the data form a projection with non linear transform applied. */
  protected ACoord getUser(AProjection2D projection) {
    currentProjection = projection;
    return projection.nonLinearTransform(getUserNoTransform(projection));
  }

  /** Should the polygon from this detector be drawn or filled? */
  protected int getDrawOrFill() {
    return AGraphics.DRAW;
  }

  /**
   * Prints info about association in which AData:index participates
   * (pick + n (navigate)
   * @param index int
   * @return String
   */
  public String navigate(int index) {
    StringBuilder temp = new StringBuilder();
    String assocKey = getFullName();
    String[] knownAssoc = event.getAssociationManager().getKnownAssociations(assocKey);
    temp.append(" associated to:\n");
    for (int i = 0; i < knownAssoc.length; i++) {
      AData source = eventManager.getCurrentEvent().get(knownAssoc[i]);
      if (source == null) continue;
      int[][] ass = event.getAssociationManager().get(assocKey, knownAssoc[i]);
      if (ass != null && ass[index] != null) {
        for (int x = 0; x < ass[index].length; x++) {
          temp.append("   " + knownAssoc[i] + " id: " + ass[index][x] +
                  " index: " + source.getIndexFromId(ass[index][x]) + "\n");
        }
      }
    }
    return temp.toString();

  } // navigate() ---------------------------------------------------------

  private boolean isEMValidate(int inputValue) {
    if (inputValue > 65535) {
      String displayMessage = "isEM is a 16-bit binary data, maximum is 65535";
      AExceptionHandler.processException("warning", displayMessage.toString());
      return false;
    }
    return true;
  }

  // "1" means a wildcard, can be "0" or "1"
  private Vector<String> getPossibleValues(String bStr) {
    Vector<String> possibleValues = new Vector<String>();
    if (bStr.length() == 1) {
      possibleValues.add("0");
      if (bStr.charAt(0) == '1') {
        possibleValues.add("1");
      }
    } else {
      Vector<String> possibleValuesWithoutFirstChar = getPossibleValues(bStr.substring(1));
      for (int i = 0; i < possibleValuesWithoutFirstChar.size(); i++) {
        String str = possibleValuesWithoutFirstChar.get(i);
        possibleValues.add("0" + str);
        if (bStr.charAt(0) == '1') {
          possibleValues.add("1" + str);
        }
      }

    }
    return possibleValues;
  }

  private void cutIsEM(String text, int[] array, Vector<String> possibleValues) {
    int num = 0;

    for (int i = 0; i < numDraw; i++) {
      for (int j = 0; j < possibleValues.size(); j++) {
        String str = possibleValues.get(j);
        int value = Integer.parseInt(str, 2);
        if (array[listdl[i]] == value) {
          listdl[num++] = listdl[i];
          break;
        }
      }
    }

    numDraw = num;
  }

  /**
   * Track name + Track storeGateKey is returned according to selected
   * Track collection in InDet -> Track -> TrackCollection listbox
   * @return String
   */
  protected String getReconstructedTracks() {
    AData tracks = eventManager.getCurrentEvent().getTrackData("InDetTrack");
    String r = null;
    if (tracks != null) {
      r = tracks.getName() + tracks.getStoreGateKey();
    }

    return r;

  } // getReconstructedTracks() ---------------------------------------------

  /**
   * Jet + Jet storeGateKey is returned according to selected
   * Jet collection in ATLAS -> Jet -> JetCollection listbox
   * @return String
   */
  protected String getJets() {
    AData jets = eventManager.getCurrentEvent().getJetData();
    String r = null;
    if (jets != null) {
      r = jets.getName() + jets.getStoreGateKey();
    }

    return r;

  } // getJets() ----------------------------------------------------------

  /**
   * RVx + RVx storeGateKey is returned according to selected
   * vertex collection in InDet -> RecVertex -> VerteCollection listbox
   * @return String
   */
  protected String getRVx() {
    AData rvx = eventManager.getCurrentEvent().getRVxData();
    String r = null;
    if (rvx != null) {
      r = rvx.getName() + rvx.getStoreGateKey();
    }

    return r;

  } // getRVx() ----------------------------------------------------------

  protected AData getObjectCollection(String type) {
    return eventManager.getCurrentEvent().getData(type);
  }

  /**
   * Cluster + Cluster storeGateKey is returned according to selected
   * Cluster collection in Calo -> Cluster -> Cluster listbox
   * @return String
   */
  protected String getClusters() {
    AData clusters = eventManager.getCurrentEvent().getClusterData();
    String r = null;
    if (clusters != null) {
      r = clusters.getName() + clusters.getStoreGateKey();
    }

    return r;

  } // getClusters() ------------------------------------------------------

  public int[] getDrawList() {
    return listdl;

  } // getDrawList() ------------------------------------------------------

    /** Apply eta+-deltaEta cut to points (e.g. S3D) */
    protected void cutEta(float[] rho, float[] z)
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        boolean useEtaCut = par.getStatus();

        if(useEtaCut)
        {
            double etaCut = Math.abs(par.getD());
            double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
            double etaLower = etaMid - etaCut;
            double etaUpper = etaMid + etaCut;
            int num = 0;

            for(int i = 0; i < numDraw; i++)
            {
                int list = listdl[i];
                double eta = AParameterUtilities.eta(z[list], rho[list]);

                if(eta - etaLower > 1.0e-6 && etaUpper - eta > 1.0e-6)
                    listdl[num++] = list;
            }
            numDraw = num;
        }
    }


    /** Apply eta+-deltaEta cut to lines perpendicular to beam axis */
    protected void cutEtaDRho(float[] rho, float[] z, float[] drho)
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        boolean useEtaCut = par.getStatus();

        if(useEtaCut)
        {
            double etaCut = Math.abs(par.getD());
            double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
            double etaLowerCut = etaMid - etaCut;
            double etaUpperCut = etaMid + etaCut;
            int num = 0;

            for(int i = 0; i < numDraw; i++)
            {
                int list = listdl[i];
                double eta1 = AParameterUtilities.eta(z[list], rho[list] + drho[list]);
                double eta2 = AParameterUtilities.eta(z[list], rho[list] - drho[list]);
                double etaLower = Math.min(eta1, eta2);
                double etaUpper = Math.max(eta1, eta2);

                if(etaUpper - etaLowerCut > 1.0e-6
                        && etaUpperCut - etaLower > 1.0e-6)
                    listdl[num++] = list;
            }
            numDraw = num;
        }
    }

    /** Apply eta+- deltaEta cut to lines parallel to beam axis */
    protected void cutEtaDZ(float[] rho, float[] z, float[] dz)
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        boolean useEtaCut = par.getStatus();

        if(useEtaCut)
        {
            double etaCut = Math.abs(par.getD());
            double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
            double etaLowerCut = etaMid - etaCut;
            double etaUpperCut = etaMid + etaCut;
            int num = 0;

            for(int i = 0; i < numDraw; i++)
            {
                int list = listdl[i];
                double etaLower = AParameterUtilities.eta(z[list] - dz[list], rho[list]);
                double etaUpper = AParameterUtilities.eta(z[list] + dz[list], rho[list]);

                if(etaUpper - etaLowerCut > 1.0e-6
                        && etaUpperCut - etaLower > 1.0e-6)
                    listdl[num++] = list;
            }
            numDraw = num;
        }
    }

    /** apply eta+-deltaeta cut to lines (e.g. TRT) */
    protected void cutEtaDEta(float[] eta, float[] deta)
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");

        if(!par.getStatus())
            return;
        double etaCut = Math.abs(par.getD());
        double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
        double etaLowerCut = etaMid - etaCut;
        double etaUpperCut = etaMid + etaCut;
        int num = 0;

        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double etaLower = eta[list] - deta[list];
            double etaUpper = eta[list] + deta[list];

            if(etaUpper - etaLowerCut > 1.0e-6
                    && etaUpperCut - etaLower > 1.0e-6)
                listdl[num++] = list;
        }
        numDraw = num;

    }



    /**
     *
     * @param sourceDatatype name of the datatype according to
     * which the cut is being done
     * @param beingCutDatatype name of the datatype which is
     * being cut
     * @param cutOption integer value of the cut option in GUI
     */
    protected void cutByAssociationTo(String sourceDatatype,
                                      String beingCutDatatype, int cutOption)
    {
        if(cutOption == 0)
        {
            // SA select all
            // do nothing, draw everything
            return;
        }

        AEvent ev = eventManager.getCurrentEvent();
        AData source = ev.get(sourceDatatype);
        int[][] assoc = event.getAssociationManager().get(beingCutDatatype, sourceDatatype);
        if (assoc == null || source == null) {
            AData being = ev.get(beingCutDatatype);
            String screenName;
            if(being!=null)screenName= being.getNameScreenName();
            else screenName="null";
            logger.warn(screenName + " - null (" + beingCutDatatype + " - " 
                    + sourceDatatype +") association, nothing to draw");
            // if 'connected' or 'unconnected' is selected and association
            // or datatype to associate with doesn't exist, draw nothing
            // (numDraw = 0)
            numDraw = 0;
            return;
        }

        if(cutOption == 1)
        {
            // SC select connected
            int num = 0;
            // first remove those not connected to anything
            for(int i = 0; i < numDraw; i++)
            {
                if(assoc[listdl[i]] != null)
                {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
            // if tracks exist apply cuts to them
            numDraw = source.cutByAssociation(numDraw, listdl, assoc);
        }
        else if(cutOption == 2)
        {
            // SU select unconnected
            int num = 0;
            for(int i = 0; i < numDraw; i++)
            {
                if(assoc[listdl[i]] == null)
                {
                    listdl[num++] = listdl[i];
                }
            }
            numDraw = num;
        }

    } // cutByAssociationTo() -----------------------------------------------



    private int cutByAssociation(int numDraw, int[] listdl, int[][] id)
    {
        // aaargh danger modifies input listdl
        makeDrawList();
        boolean[] drawn = isDrawn();
        int num = 0;

        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            if(id[list] != null)
            {
                for(int j = 0; j < id[list].length; ++j)
                {
                    int inverse = indexFromId.get(id[list][j]);
                    if(inverse != NO_INVERSE && drawn[inverse])
                        listdl[num++] = list;
                    break;
                }
            }
        }
        numDraw = num;
        return numDraw;
    }

    /**
     * return graphics attributes corresponding to layer =0 frame layer =1 hits
     * or tracks type=0 noise type=1 non-noise
     */
    public ADrawParameters getDrawParameters(int layer, int type)
    {
        boolean draw = true;
        int singleColor = -1;
        int size = 10;
        int lineWidth = 1;
        int symbol = 0;
        int frameWidth = 0;
        boolean forceSymbols = false;
        int minSize = 0;

        AParameter forceSymbolsPar = parameterStore.getUnknown(PARAMETER_GROUP, "ForceSymbols");
        AParameter symbolSize = parameterStore.getUnknown(PARAMETER_GROUP, "SymbolSize");
        AParameter symbolType = parameterStore.getUnknown(PARAMETER_GROUP, "Symbol");
        AParameter lineWidthPar = parameterStore.getUnknown(PARAMETER_GROUP, "LineWidth");

        if(forceSymbolsPar != null)
        {
            forceSymbols = forceSymbolsPar.getStatus();
            if(forceSymbols && symbolSize != null)
                minSize = Math.max(symbolSize.getI(), 1);
        }

        if(symbolSize != null)
            size = symbolSize.getI();
        if(symbolType != null)
            symbol = symbolType.getI();
        if(lineWidthPar != null)
            lineWidth = lineWidthPar.getI();
        //for etmiss, line width scaled by energy by using the stroke setting
        if(lineWidth<=0)
            lineWidth = 1;

        if(type == 0)
        {
            AParameter noiseSymbolType = parameterStore.getUnknown(PARAMETER_GROUP, "Noise");

            if(noiseSymbolType != null && noiseSymbolType.getStatus())
            {
                AParameter noiseSymbolSize = parameterStore.getUnknown(PARAMETER_GROUP, "NoiseSize");
                AParameter noiseWidth = parameterStore.getUnknown(PARAMETER_GROUP, "NoiseWidth");

                if(noiseSymbolSize != null)
                    size = noiseSymbolSize.getI();
                if(noiseWidth != null)
                    lineWidth = noiseWidth.getI();
                symbol = noiseSymbolType.getI();
            }
            else
            {
                AParameter noiseWidth = parameterStore.getUnknown(PARAMETER_GROUP, "NoiseWidth");

                if(noiseWidth != null && noiseWidth.getStatus())
                    lineWidth = noiseWidth.getI();
            }
        }

        if(layer == 0)
        {
            AParameter frameWidthPar = parameterStore.getUnknown(PARAMETER_GROUP, "FrameWidth");
            if(parameterStore.get(PARAMETER_GROUP, "Frame")!=null)
            {
                boolean drawFrame = parameterStore.get(PARAMETER_GROUP, "Frame").getStatus();
                singleColor = parameterStore.get(PARAMETER_GROUP, "Frame").getI();
                //only draw frames for Grey/BW color maps if is selected to draw frames
                if(drawFrame && AColorMap.drawFrames())    
                {
                    if(frameWidthPar != null)
                        frameWidth = frameWidthPar.getI();
                }
            }
        }
        return new ADrawParameters(draw, singleColor, size, lineWidth,
                frameWidth, symbol, forceSymbols, minSize, getDrawOrFill());
    }

    /** Provide an array of indices with increasing a,
     *  used e.g. by coloring.
     *  @param a
     *  @return
     */
    protected int[] indexBy(double[] a)
    {
        int[] index = new int[a.length];

        for(int i = 0; i < a.length; i++)
            index[i] = i;
        // this is only cosmetic improvement in appearance so if input data too
        // large simply return null
        if(a.length > 1000)
            return index;

        for(int i = 0; i < a.length - 1; i++)
            for(int j = i + 1; j < a.length; j++)
                if(a[index[i]] > a[index[j]])
                {
                    int temp = index[i];
                    index[i] = index[j];
                    index[j] = temp;
                }
        return index;
    }

    /**
     * Colour items in this class by the associated class that is given by 
     * association. The difference with the method colorByAssociation is that 
     * colorBy is called on the target class, while colorByAssociation is called 
     * on the source class of the colour. (i.e. colour hits by tracks invokes
     * AHitData.colorBy("Tracks"), calling ATrackData.colourByAssociation(...)
     * internally.
     *
     * @param otherClass source class for the colours 
     */
    protected void colorBy(String otherClass)
    {
        AData source = eventManager.getCurrentEvent().get(otherClass);
        byte unconnected = (byte) parameterStore.get(PARAMETER_GROUP, "Unconnected").getI();
        if(source != null)
        {
            byte shared = (byte) parameterStore.get(PARAMETER_GROUP, "Shared").getI();
            String assocKey = getFullName();
            int[][] assoc = event.getAssociationManager().get(otherClass, assocKey); 
            source.makeDrawList();
            source.color();
            source.colorByAssociation(this, assoc, unconnected, shared);

        }
        else
        {
            /*
             * given association doesn't exist. for instance, there are only
             * hits in the event file and nothing to associate them with.
             * if some 'association colouring' (e.g. colour by reconstructed
             * track) is selected in that case, the items appear all in
             * 'unconnected' colour. another option is to colour them in
             * constant colour, simply by calling only colorByConstant();
             */
            for(int i = 0; i < numData; i++)
            {
                color[i] = (byte) unconnected;
            }
        }

    } // colorBy() ----------------------------------------------------------

    protected void colorByObjects()
    {
        // initially set all with unconnected color, connected will be changed later
        byte unconnectedColor = (byte) parameterStore.get(PARAMETER_GROUP, "Unconnected").getI();
        setColor(unconnectedColor);
        String[] objectList = new String[] {"Jet", "BJet", "TauJet", "Photon", "Electron", "Muon"};
        for(int i=0; i<objectList.length; ++i)
        {
            if (parameterStore.get("Data", objectList[i]).getStatus())
                colorByObject(getObjectCollection(objectList[i]));
        }
    }

    private void colorByObject(AData objectData)
    {
        if(objectData == null)
        {
            return;
        }

        AEnumeratorParameter listBox = (AEnumeratorParameter) parameterStore.get(getName(), getName() + "Collections");
        String selectedCollection = listBox.getCurrentText();
        if(!("All".equals(selectedCollection) || "None".equals(selectedCollection)))
        {
            String assName = getName();
            if(getName().indexOf("Track") >= 0)
                assName = "Track";
            AAssociation assoc = event.getAssociationManager().getAssociation(objectData.getName()+objectData.getStoreGateKey(), assName);
            if(assoc != null)
            {
                if(!(assoc instanceof AObjectsAssociation))
                {
                    logger.warn(getClass().getName() + ":  no objects association is found!");
                    return;
                }
                int[][] associatedIndex = ((AObjectsAssociation) assoc).getData();
                String[] associatedKey = ((AObjectsAssociation) assoc).getKey();

                // if currently selected collection is included in associatedKey, show associations
                // otherwise using the color set for unconnected cluster/track
                int objectIndex = 0;
                for(int i=0; i<associatedKey.length;)
                {
                    // objectIndex is the index of the associated object (e.g. Electron)
                    // associatedIndex[objectIndex][] contain the index of the associated Cluster/Track
                    if ("none".equals(associatedKey[i]))
                    {
                        i++;
                        objectIndex++;
                    }
                    else
                    {
                        if (associatedKey[i].equals(selectedCollection))
                            for(int j=0; j<associatedIndex[objectIndex].length; ++j)
                                setColor(associatedIndex[objectIndex][j], objectData.getColor()[objectIndex]);
                        i+=associatedIndex[objectIndex++].length;
                    }
                }
            }
        }
    }

    /**
     * The same as name, but if it's multiple collection datatype, the full
     * name is getName() + getStoreGateKey()
     * @return String
     */
    public String getFullName()
    {
        String k = this.getStoreGateKey();
        String full = this.getName() + (k != null ? k : "");
        return full;
    }

    public int getIdFromIndex(int index)
    {
        return id[index];
    }

    // sets the color of each hit
    protected final void color()
    {
        // standard scheme
        internalColor();
        int[][] temp = AListManager.getInstance().getColorMapping(this);
        int[] index = temp[0];
        int[] c = temp[1];
        // now add in colors specified in lists

        for(int i = 0; i < index.length; ++i)
            if(c[i] >= 0)
                color[index[i]] = (byte) c[i];
        int others = AListManager.getInstance().getColorOfOthers();
        // need to check if this data could have been picked
        // so that coloring of hits by STr works even if STr
        // is not in list because it wasn't on....
        if(others >= 0 && parameterStore.get("Data", getName()).getStatus())
        {
            boolean[] inList = new boolean[numData];
            for(int i = 0; i < index.length; ++i)
                inList[index[i]] = true;

            for(int i = 0; i < numData; ++i)
                if(!inList[i])
                    color[i] = (byte) others;
        }



    }


    /** Type = e.g. noise/good */
    public int[] getType(int[] dl)
    {
        return new int[dl.length];
    }

    // get data representation in user space in different projections
    // empty by default
    protected ACoord getYXUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getRZUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getYZUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getXZUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord get3DUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getUserUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getUserUser2()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getFRUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getFZUser()
    {
        return ACoord.NO_DATA;
    }

    protected ACoord getVPUser()
    {
        return ACoord.NO_DATA;
    }

    public int getNumTypes()
    {
        return 1;
    }


    /** The only method from AListProcessor interface. */
    @Override
    public Action[] getActions(Collection nodes)
    {
        return new Action[0];
    } // getActions() -------------------------------------------------------

    /**
     * Function to split a list of items seperated by -
     */
    public String[] splitItems(String origItems)
    {
        char[] origItemsChar = origItems.toCharArray();
        int origItemsLength = origItems.length();
        int spacePosition=0, noOfItems=0, count=0;
        //calculate the number of items
        for(int i=0;i<origItemsLength;i++)
            if((i!=0 && origItemsChar[i]=='-') || (i==origItemsLength-1 && origItemsChar[i]!='-'))
                noOfItems++;
        //split up the array into newItems
        if(noOfItems>0)
        {
            //string array to hold individual items
            String[] newItems=new String[noOfItems];
            for(int i=0;i<origItemsLength;i++)
            {
                if(i==0 && origItemsChar[i]=='-')
                    spacePosition=i+1;//ignore first'-'
                else if(i==origItemsLength-1 && origItemsChar[i]!='-')
                {
                    //store the text from previous '-' upto end if doesn't end with '-'
                    newItems[count]=origItems.substring(spacePosition,i+1);
                    spacePosition=i+1;
                    count++;                    
                }
                else if(origItemsChar[i]=='-' && i!=0)
                {
                    //store the text from previous '-' upto current '-'
                    newItems[count]=origItems.substring(spacePosition,i);
                    spacePosition=i+1;
                    count++;
                }
            }
            return newItems;
        }
        else
            return null;
    }   

}
