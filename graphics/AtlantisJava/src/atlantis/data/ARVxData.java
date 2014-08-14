/*
 * CAUTION: When working on this class, remember that the number of vertices 
 *          is not a fixed number for an event. If the user fits additional 
 *          vertices in Atlantis, they will be appended to the list. For this 
 *          reason everything is using ArrayList and not simple arrays.
 */

package atlantis.data;

import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.tree.DefaultMutableTreeNode;

import atlantis.canvas.ACanvas;
import atlantis.event.AEvent;
import atlantis.event.AData;
import atlantis.event.AAssociation;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.list.AList;
import atlantis.list.AListManager;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection3D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.ALogger;

import static atlantis.data.AVertex.Type.*;

/**
 * Stores Reconstructed vertices formed for fitting groups
 * of tracks. There may not be any reconstructed vertices when a new event
 * is read in. However, empty datatype ARVxData is always created within
 * the AEvent instance. RVx instances may be added at run time by the user
 * when the vertex fitter algorithm is run inside Atlantis.
 * 
 * @author Gary Taylor
 */
public class ARVxData extends AData
{
    private static ALogger logger = ALogger.getLogger(ARVxData.class);
    
    // size of the arrays with coordinates of the RVx ellipse,
    // size of martices
    private static final int SIZE = 3;

    // vertices - saved reconstructed vertices, instances of AVertex
    private ArrayList<AVertex> vertices = new ArrayList<AVertex>();
    // tracksIndex - indices of tracks (starting from 0)
    private ArrayList<int[]> tracksIndex = new ArrayList<int[]>();
    // tracksId - IDs of tracks (as read from event file)
    private ArrayList<int[]> tracksId = new ArrayList<int[]>();
    private ArrayList<String> tracksCollection = new ArrayList<String>();
    private ArrayList<Boolean> drawFlag = new ArrayList<Boolean>();
    // false - RVx calculated in Atlantis, true - RVx calculated in Athena
    private ArrayList<Boolean> athenaOrigin = new ArrayList<Boolean>();

    // array of flags 1 - is primary vertex candidate, this
    // attribute should be accessed only during event finalise phase
    private int[] primVxCand = null;
    
    /** Vertex type from Athena: for types see
     * http://alxr.usatlas.bnl.gov/lxr/source/atlas/Tracking/TrkEvent/TrkEventPrimitives/TrkEventPrimitives/VertexType.h */
    private int[] vertexType = null;

    /** Array containing number of tracks associated with each vertex. */
    private int[] numTracks = null;
    
    /** Flag whether track numbers are consistent: see Trac bug #551 */
    private boolean hasConsistentTrackNumbers;

    // numData (as at all other datatypes) is used instead of numVertices
    // numData variable is referenced from ADrawnGraphics2D (by .getNumData
    // method) and since it wasn't set properly in case of RVx it was crashing
    // in ADrawnGraphics2D.drawLine() [when rubberbanding over RVx object]

    public String getParameterGroup()
    {
        return "RVx";
    }

    @Override
    public String getName()
    {
        return "RVx";
    }

    public String getNameScreenName()
    {
        return "RecVertex";
    }

    /**
     * This constructor is called when RVx datatype is found in the event file.
     * This RVx data was produced in Athena.
     * @param p elements within RVx element in JiveXML input: map of names to values 
     * @param e parent event
     * @throws AAtlantisException if failing to create data
     */
    ARVxData(AHashMap p, AEvent e) throws AAtlantisException
    {
    	super(p,e);

    	int[] tracks = p.getIntArray("tracks");
    	numTracks = p.getUnsureIntArray("numTracks");
    	if (numTracks == null) numTracks = new int[numData];
    	float[] z = p.getFloatArray("z");
    	float[] y = p.getFloatArray("y");
    	float[] x = p.getFloatArray("x");
    	float[] covMatrix = p.getFloatArray("covMatrix");
    	float[] chi2 = p.getFloatArray("chi2");
    	String[] storegateKeys = null;

    	// Check whether numbers of tracks agree.
    	hasConsistentTrackNumbers = false;
    	if (tracks!=null) {
    		int numTrackIds = tracks.length;
    		int numTracksFromVertices = 0;
    		for (int numTracksFromVertex : numTracks) {
    			numTracksFromVertices += numTracksFromVertex;
    		}
    		if (numTracksFromVertices == numTrackIds) {
    			hasConsistentTrackNumbers = true;
    		}
    	}
    	if (!hasConsistentTrackNumbers) {
    		logger.error("RVx: numbers of tracks are inconsistent.");
    	}

    	if(p.getStringArray("sgkey") != null)
    	{
    		storegateKeys = p.getStringArray("sgkey");
    	}
    	else
    	{
    		storegateKeys = new String[numData];
    		for (int i = 0; i < numData; i++)
    		{
    			storegateKeys[i] = "n/a";
    		}
    	}

    	// Allowed to be null if element not in JiveXML.
    	vertexType = p.getUnsureIntArray("vertexType");
    	
    	primVxCand = p.getUnsureIntArray("primVxCand");
    	if (primVxCand == null) primVxCand = new int[numData];

    	if (hasConsistentTrackNumbers) createVertexTrackAssociations(tracks, storegateKeys);

    	double[] pos = new double[SIZE];
    	double[][] cov = new double[SIZE][SIZE];
    	int[] tracksIdArray = null;
    	int[] tracksIndexArray = null;

    	int n = 0;
    	int m = 0;
    	for (int i = 0; i < numData; i++)
    	{
    		// x, y, z coordinates of the RVx ellipse
    		pos[0] = x[i];
    		pos[1] = y[i];
    		pos[2] = z[i];

    		// covMatrix, resp. 3x3 cov array
    		// numbers are too small, so JiveXML multiplies by 10k, here inverse
    		// [the same happens with covMatrix for tracks - AHelixData class]
    		for (int j = 0; j < SIZE; j++)
    		{
    			for (int k = 0; k < j + 1; k++)
    			{
    				cov[j][k] = covMatrix[n] / 10000;
    				cov[k][j] = cov[j][k];
    				n++;
    			}
    		}

    		AVertex vertex = new AVertex(chi2[i], pos, cov);
    		vertices.add(vertex); // new vertex

    		// Only save track details if data are consistent.
    		if (hasConsistentTrackNumbers) {
    			tracksIdArray = new int[numTracks[i]];
    			tracksIndexArray = new int[numTracks[i]];
    			for (int j = 0; j < numTracks[i]; j++)
    			{
    				tracksIdArray[j] = tracks[m];
    				// should be removed - problem that IDs run from 1 and indices
    				// from 0 - will be fixed
    				tracksIndexArray[j] = tracks[m] - 1;
    				m++;
    			}
    			tracksIndex.add(tracksIndexArray.clone()); // save indices of tracks
    			tracksId.add(tracksIdArray.clone()); // save IDs of tracks
    		}
    		
    		// save storeGateKey of the Tracks which form this vertex
    		tracksCollection.add(storegateKeys[i]);
    		drawFlag.add(new Boolean(true));
    		athenaOrigin.add(new Boolean(true));
    	} // for(int i = 0; i < numData; i++)
    } // ARVxData() ---------------------------------------------------------


    /**
     * Create vertex-track associations. and adds them to the event's association manager.
     * @param tracks array of track ID numbers
     * @param storegateKeys array of storegate keys, one per vertex
     * Also uses numTracks: number of tracks associated with each vertex.
     */
	private void createVertexTrackAssociations(int[] tracks, String[] storegateKeys) {
		// Create a list of unique storegate keys.
    	Vector<String> uniqueStoregateKeys = new Vector<String>();
    	if(!storegateKeys[0].equals("n/a")){ 
    		uniqueStoregateKeys.add(storegateKeys[0]);
    	}
    	for(int i = 0; i < storegateKeys.length; i++){
    		if(storegateKeys[i].equals("n/a")){
    			continue;
    		}
    		// Check if track particle type already added to list of storegate keys
    		boolean inVector = false;
    		for (String sgKey : uniqueStoregateKeys) {
    			if (sgKey.equals(storegateKeys[i])) {
    				inVector = true;		    
    				break;
    			}
    		}
    		// If storegate key is not in the list, it is added. 
    		if(!inVector){
    			uniqueStoregateKeys.add(storegateKeys[i]);
    		}
    	}
    	// Create new association int array for each unique storegate key.
    	for(int i = 0; i < uniqueStoregateKeys.size(); i++){
        	int trackCount = 0;
        	Vector<Integer> TrkNum = new Vector<Integer>();
        	Vector<Integer> Track = new Vector<Integer>();
    		// Creating vectors of the 'number of tracks associated to each list' and 'track Ids'
    		for(int j = 0; j < storegateKeys.length; j++){
    			if(uniqueStoregateKeys.get(i).equals(storegateKeys[j])){
    				TrkNum.add(numTracks[j]);
    				for(int k = trackCount; k < trackCount + numTracks[j]; k++){
    					Track.add(tracks[k]);
    				}
    			}
    			trackCount += numTracks[j];
    		}
    		//Converting vectors to int[]
    		int[] assocTracks =  new int[Track.size()];
    		for(int j = 0; j < Track.size(); j++){
    			assocTracks[j] =  Track.get(j);
    		}
    		int[] numTrks = new int[TrkNum.size()];
    		for(int j = 0; j < TrkNum.size(); j++){
    			numTrks[j] = TrkNum.get(j);
    		}
    		//Creating associations
                AAssociation assoc = new AAssociation("RVx" + getStoreGateKey(),
                        "InDetTrack" + uniqueStoregateKeys.get(i), numTrks, 
                        assocTracks, event);
    		event.getAssociationManager().add(assoc);
    	}
	}

    /**
     * Redefined inherited method AData.getIdFromIndex()
     * the array int[] id is not used in this class (unlike other types) so
     * AData.getIdFromIndex() was crashing at RVx since this array was empty
     * listdl[] array (also inherited from AData) should be properly handled
     * in this class (used to crash when rubberbanding over displayed RVx)
     * @param index int
     * @return int
     */
    @Override
    public int getIdFromIndex(int index)
    {
        return listdl[index];
    } // getIdFromIndex() -----------------------------------------------------

    /**
     *
     * @param listOfTracksToVertex int[] - list of track indices (internal
     * Atlantis indices starting from 0)
     * @param key - Track collection storeGateKey
     */
    private void createVertexFromTracks(int[] listOfTracksToVertex, String key)
    {

	//Retrieving/checking for pre-existing assocation
	int[][] assoc = event.getAssociationManager().get("RVx","InDetTrack" + key);
	if(assoc == null){
	    //Create new association for the between selected tracks and the new vertex
	    int[] numTracks2 = new int[1];
	    numTracks2[0] = listOfTracksToVertex.length;
	    event.getAssociationManager().add(new AAssociation("RVx","InDetTrack" + key, numTracks2, listOfTracksToVertex, event));
	}else{//if assocation already exists add new one to it 	   
	    //Remove old assocation 
	    event.getAssociationManager().remove("InDetTrack" + key + "RVx");
	    //Create new assocation by copying old one and adding extra line at the end.
	    int[][] newAssoc = new int[assoc.length + 1][];
	    for(int i = 0; i < newAssoc.length; i++){
		if(i == assoc.length){
		    newAssoc[i] = listOfTracksToVertex;
		}else{
		    newAssoc[i] = assoc[i];
		}
	    }
	    //recreating  assocation
	    event.getAssociationManager().add(new AAssociation("RVx","InDetTrack" + key,  newAssoc, event));
	}



        ATrackData aRTr = null;
        aRTr = event.getTrackData("InDetTrack", key);
        if (aRTr == null)
        {
            return;
        }
        // get helices of the appropriate tracks, helices are access by indices
        // save the IDs of tracks into trackIdLocal which will be copied to tracksId
        int[] trackIdLocal = new int[listOfTracksToVertex.length];
        AHelix[] thisHelGoToFitter = new AHelix[listOfTracksToVertex.length];
        for (int j = 0; j < listOfTracksToVertex.length; j++)
        {
            AHelix helix = aRTr.getModifiableHelix(listOfTracksToVertex[j]);
            if (helix == null) {
                AOutput.alwaysAppend("\nOne or more of the selected tracks do not have perigee parameters, "
                               + "the vertex cannot be created\n", ALogInterface.BAD_COMMAND);
                return;
            }
            thisHelGoToFitter[j] = helix;
            trackIdLocal[j] = aRTr.getIdFromIndex(listOfTracksToVertex[j]);
        }


        double xVtx = parameterStore.get("Event", "XVtx").getD();
        double yVtx = parameterStore.get("Event", "YVtx").getD();
        double zVtx = parameterStore.get("Event", "ZVtx").getD();
        AVertex startingPoint = new AVertex(new double[] { xVtx, yVtx, zVtx });
        try
        {
            // RuntimeException is thrown - no vertex found or goes on (new vertex)
            AVertex vertex = AVertexFit.fitVertex(startingPoint, thisHelGoToFitter);
            vertices.add(vertex); // new vertex
            tracksIndex.add(listOfTracksToVertex.clone()); // save indices of tracks
            tracksId.add(trackIdLocal.clone()); // save IDs of tracks
            tracksCollection.add(key);
            drawFlag.add(new Boolean(true));
            athenaOrigin.add(new Boolean(false));

            int[] newNumTracks = new int[numData+1];
            int[] newPrimVxCand = new int[numData+1];
            for (int i=0; i<numData; i++) {
                newNumTracks[i] = numTracks[i];
                newPrimVxCand[i] = primVxCand[i];
            }
            numTracks = newNumTracks;
            primVxCand = newPrimVxCand;
            numTracks[numData] = listOfTracksToVertex.length;
            primVxCand[numData] = 2;

            if (vertexType != null) {
                int[] newVertexType = new int[numData+1];
                for (int i=0; i<numData; i++) {
                    newVertexType[i] = vertexType[i];
                }
                vertexType = newVertexType;
                vertexType[numData] = 2;
            }

            double[] par = vertex.getPosition();
            double Vx = par[0];
            double Vy = par[1];
            // temp set axis and orientation of 3D rotation wrt last vertex
            // adjust the starting point of the tracks according to the vertex
            double Vz = par[2];
            for (int j = 0; j < thisHelGoToFitter.length; j++)
            {
                double a = Vx - xVtx;
                double b = Vy - yVtx;
                double c = Vz - zVtx;
                double size = Math.sqrt(a * a + b * b + c * c);
                parameterStore.get("3D", "xAxis").setD(a / size);
                parameterStore.get("3D", "yAxis").setD(b / size);
                parameterStore.get("3D", "zAxis").setD(c / size);
                thisHelGoToFitter[j].setPhiStartByXYPoint(Vx, Vy);
            }
            AOutput.alwaysAppend("\n" + getHitInfo(numData) + "\n", ALogInterface.NORMAL);
            numData++;
            makeDrawList();
        }
        catch (RuntimeException e)
        {
            StringBuilder temp = new StringBuilder("\nRVx: tracks (");
            for (int i = 0; i < listOfTracksToVertex.length; i++)
            {
                if (i > 0)
                {
                    temp.append(", ");
                }
                // the tracks were accessed by indices but user wants to see
                // IDs like pick prints (listOfTracksToVertex[] - indices)
                temp.append(aRTr.getIdFromIndex(listOfTracksToVertex[i]));
            }
            AOutput.alwaysAppend(temp + ")\n   not originating from a common vertex\n", ALogInterface.NORMAL);
            throw e;
        }
    } // createVertexFromTracks() -------------------------------------------

    /**
     * 'Colour by vertex type' option, that allows to colour the vertices depending on whether they are primary,
     *  secondary or other vertices.
     */
    protected void colorbyVertexType() {
 		
		  int pri = parameterStore.get("RVx", "PriVxColor").getI();
		  int sec = parameterStore.get("RVx", "SecVxColor").getI();
		  int res = parameterStore.get("RVx", "RestVxColor").getI();
	     
		  if (vertexType!=null) {
			  for (int i=0; i<numData; ++i) {
				  int typeId = vertexType[i];
				  AVertex.Type type = AVertex.Type.typeFromId(typeId);
				  switch (type) {
				  case PRIMARY:
				  case DUMMY:
					  color[i] = (byte) pri;
					  break;
				  case SECONDARY:
					  color[i] = (byte) sec;
					  break;
				  case TEMPV0LAMBDA:
					  color[i] = (byte) sec;
					  break;
				  case TEMPV0LAMBDABAR:
					  color[i] = (byte) sec;
					  break;
				  case TEMPKSHORT:
					  color[i] = (byte) sec;
					  break;
				  case PILEUP:
				  case CONVERSION:
				  case V0:
				  case KINK:
					  color[i] = (byte) res;
					  break;
				  default:
					  color[i] = (byte) res;  
					  							    					  					  									  
				  }
			  }
		  }
		  else if (primVxCand!=null) {          // use primary vertex candidate flag as fall-back if vertex type not available
              for (int i=0; i<numData; ++i) {
                  if (primVxCand[i]==1) {
                      color[i] = (byte) pri;
                  }
                  else {
                      color[i] = (byte) res;
                  }
              }
		  }
		  else {                               // if no vertex type information, use constant colour 
		      colorByConstant();
		  }
	  }
    
    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        // The number of vertices can increase when the user fits a vertex in
        // Atlantis, make sure the color array also increases in size then
        if (numData > color.length)
            color = new byte[numData];

        if (colorFunction == 0)
        {
            colorByConstant();
        }
        else if(colorFunction == 1)
        {
            colorByIndex();
        }
        else if (colorFunction == 2)       	
        {       
        	colorbyVertexType();
        }
        return 0;
    } // internalColor() ----------------------------------------------------

    @Override
    public void makeDrawList()
    {	    
	    numDraw = 0;
	    listdl = new int[numData];
	    for (int i = 0; i < numData; i++) {
            if (drawFlag.get(i).booleanValue()) {
                listdl[numDraw] = i;
				numDraw++;
            }           	           	          	
        }
        this.cutTracksByVertexType();
        this.applyCuts();
    }

    private void cutTracksByVertexType() {
        numDraw = 0;
        listdl = new int[numData];
        for (int i = 0; i < numData; i++) {
            if (drawFlag.get(i).booleanValue()) {
                // All=0, Primary=1, Secondary=2, Rest=3
                int vertexTypeSelected =  parameterStore.get("CutsObjects", "vertextype").getI();
                if (vertexTypeSelected==0) {
                    listdl[numDraw] = i;
                    numDraw++;
                }
                if (vertexTypeSelected==1) { // primary
                    if (vertexType!=null) {
                        int typeId = vertexType[i];
                        AVertex.Type type = AVertex.Type.typeFromId(typeId);
                        if (type == PRIMARY || type == DUMMY){
                            listdl[numDraw] = i;
                            numDraw++;
                        }
                    }
                }
                if (vertexTypeSelected==2){
                    if (vertexType!=null) {
                        int typeId = vertexType[i];
                        AVertex.Type type = AVertex.Type.typeFromId(typeId);
                        if (type == SECONDARY || type == TEMPV0LAMBDA || type == TEMPV0LAMBDABAR || type == TEMPKSHORT){
                            listdl[numDraw] = i;
                            numDraw++;
                        }
                    }
                }
                if (vertexTypeSelected==3){
                    if (vertexType!=null) {
                        int typeId = vertexType[i];
                        AVertex.Type type = AVertex.Type.typeFromId(typeId);
                        if (type == PILEUP || type == CONVERSION || type == V0 || type == KINK){
                            listdl[numDraw] = i;
                            numDraw++;
                        }
                    }
                }              	    
            }
        }
    }
    
    public String getHitInfo(int index)
    {
        boolean athenaOriginBoolean = athenaOrigin.get(index).booleanValue();
        String origin = (athenaOriginBoolean == true) ? "Athena" : "Atlantis";
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
        if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        "\n calculated in "+origin;
        
        StringBuilder temp = new StringBuilder();
        temp.append("\n" + getNameScreenName() + " (index: " + index + ") (calculated in " + origin + ")\n" + " type of Track: " + tracksCollection.get(index) + "\n");
        temp.append(" tracks IDs = ");
        boolean tracksIDsKnown = false;
        if (tracksId.size() > 0) {
            int[] tracksIdInVertex = (tracksId.get(index));
            for (int i = 0; i < tracksIdInVertex.length; i++)
            {
                int id2 = tracksIdInVertex[i];
                // if the JiveXML knows that vertex creates X number of
                // tracks but doesn't know the ID, -1 is put as tracks ID
                // list of -1 doesn't need to be printed
                if (id2 > -1)
                {
                    tracksIDsKnown = true;
                    if (i > 0)
                    {
                        temp.append(",");
                    }
                    temp.append(tracksIdInVertex[i]);
                }
            }
        }
        if (!tracksIDsKnown)  temp.append("n/a");
        //To display in the output screen the vertex type number and description when this is selected
        if (vertexType!=null) {
			  int typeId = vertexType[index];
			  AVertex.Type type = AVertex.Type.typeFromId(typeId);
			  String vertexInfo = String.format("%nVertex type = %d (%s)", typeId, type.description);
			  temp.append(vertexInfo);
        }
        temp.append("\nPrimary candidate status = " + primVxCand[index]);
        temp.append("\nnumTracks = " + numTracks[index]);
        temp.append("\n" + vertices.get(index));
        return temp.toString();
    } // getHitInfo() -------------------------------------------------------

    @Override
    public ACoord getYXUser()
    {
        makeDrawList();
        double hv[][][] = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double[][] p = vertex.getYXEllipse();
            hv[0][i] = p[0];
            hv[1][i] = p[1];

            index[i] = listdl[i];
        }
        return new ACoord(hv, index, this);
    } // getYXUser() --------------------------------------------------------

    @Override
    public ACoord getXZUser()
    {
        makeDrawList();
        double hv[][][] = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double[][] p = vertex.getXZEllipse();
            hv[0][i] = p[0];
            hv[1][i] = p[1];
            index[i] = listdl[i];
        }
        return new ACoord(hv, index, this);
    } // getXZUser() --------------------------------------------------------

    @Override
    public ACoord getYZUser()
    {
        makeDrawList();
        double hv[][][] = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double[][] p = vertex.getYZEllipse();
            hv[0][i] = p[0];
            hv[1][i] = p[1];
            index[i] = listdl[i];
        }
        return new ACoord(hv, index, this);
    } // getYZUser() --------------------------------------------------------

    @Override
    public ACoord getRZUser()
    {
        makeDrawList();
        double hv[][][] = new double[2][numDraw][];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            //TODO: make "RZ ellipse"
            double[][] p = vertex.getRZEllipse();
            hv[0][i] = p[0];
            hv[1][i] = p[1];
            index[i] = listdl[i];
        }
        return new ACoord(hv, index, this);
    } // getRZUser() --------------------------------------------------------

    @Override
    protected ACoord get3DUser()
    {
        makeDrawList();
        double h[] = new double[numDraw];
        double v[] = new double[numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            double[] hvo = AProjection3D.getRotated(vertices.get(listdl[i]).getPosition());
            h[i] = hvo[0];
            v[i] = hvo[1];
            index[i] = listdl[i];
        }
        return new ACoord(h, v, index, this);
    } // get3DUser() --------------------------------------------------------

    @Override
    public ACoord getFZUser()
    {
        makeDrawList();
        double h[] = new double[numDraw];
        double v[] = new double[numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double[] xyz = vertex.getPosition();
            double phi = vertex.getPhi();
            double z = xyz[2];
            h[i] = z;
            v[i] = Math.toDegrees(phi);
            index[i] = listdl[i];
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FZ");
    } // getFZUser() --------------------------------------------------------

    @Override
    public ACoord getFRUser()
    {
        makeDrawList();
        double h[] = new double[numDraw];
        double v[] = new double[numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double rho = vertex.getRho();
            double phi = vertex.getPhi();
            h[i] = rho;
            v[i] = Math.toDegrees(phi);
            index[i] = listdl[i];
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FR");
    } // getFRUser() --------------------------------------------------------

    @Override
    public ACoord getVPUser()
    {
        makeDrawList();
        double h[] = new double[numDraw * 2];
        double v[] = new double[numDraw * 2];
        int[] index = new int[numDraw * 2];
        double[] sign = new double[] { -1., 1. };
        int num = 0;

        for (int i = 0; i < numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            double[] xyz = vertex.getPosition();
            // if rho is 0, AMath.eta() divides by 0, since it's real type,
            // Infinity is returned, program doesn't crash, vertex is not drawn
            // proper would be to test all rho > 0.01cm values in advance
            double rho = vertex.getRho();
            double phi = vertex.getPhi();
            double z = xyz[2];
            double eta = AParameterUtilities.eta(z, rho);
            double delEta = AProjectionVP.getDeltaEta(rho, z);
            for (int j = 0; j < 2; j++)
            {
                h[num] = eta + sign[j] * delEta;
                v[num] = Math.toDegrees(phi);
                index[num] = listdl[i];
                num++;
            }
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("VP");
    } // getVPUser() --------------------------------------------------------

    protected void applyCuts()
    {
        cutIndex();

        float[] phi = new float[numDraw];
        float[] eta = new float[numDraw];
        for(int i=0; i<numDraw; i++)
        {
            AVertex vertex = vertices.get(listdl[i]);
            phi[i] = (float) vertex.getPhi();
            double z = vertex.getPosition()[2];
            eta[i] = (float) AParameterUtilities.eta(z, vertex.getRho());
        }
        cutPhi(phi);
        cutEta(eta);

        cut("CutsInDet", "NumRVxTracks", " Ntrack", numTracks);
        cut("CutsInDet", "PrimaryRVx", " Primary", primVxCand);

    } // applyCuts() --------------------------------------------------------

    /**
     * @param nodes Collection
     * @return Action[]
     * Method returns either empty list of Actions or Vertex item after the
     * right mouse click on the list (in the list dialog)
     * Vertex only appear if there is one type of reconstructed tracks in the
     * list and no other datatype
     */
    @Override
    public Action[] getActions(Collection nodes)
    {
        if (nodes.size() != 1)
        {
            return new Action[0];
        }
        final DefaultMutableTreeNode node = (DefaultMutableTreeNode) (nodes.iterator().next());
        if (!node.getAllowsChildren())
        {
            return new Action[0];
        }
        AList[] list = AListManager.getInstance().getChildren(node);
        if (list == null)
        {
            return new Action[0];
        }
        Set<Integer> tracks = new HashSet<Integer>();
        Set<Integer> tracksID = new HashSet<Integer>();
        // iterate over the items in the list (set of whichever datatypes)
        // condition in this loop assures that 'Vertex *' action appears only
        // if there is/are set(s) of one type of reconstructed tracks and no
        // other types. the condition used to be
        // list[i].getSource() == ... .event.getRTrData() which was
        // a method depending on the current selected Hits-to-RTr association
        for (int i = 0; i < list.length; i++)
        {
            // tests that there is either type of reconstructed tracks in the list
            if (list[i].getSource() instanceof ATrackData)
            {
                // test that there is just one type of reconstructed tracks if
                // there are more than one item in list
                if (i > 0)
                {
                    String key1 = list[i - 1].getSource().getStoreGateKey();
                    String key2 = list[i].getSource().getStoreGateKey();
                    if (!(key1 == null && key2 == null) && !key1.equals(key2))
                    {
                        String msg = "info: Vertexing is available if only one " +
                                     "Track collection is chosen in the list. " +
                                     " (select: InDet->Track->Track Collections)\n";
                        AOutput.alwaysAppend(msg, ALogInterface.NORMAL);
                        return new Action[0];
                    }
                }
                // tests passed - save indices of the tracks from the hashtable
                for (int j = 0; j < list[i].getItems().length; j++)
                {
                    int index = list[i].getItems()[j];
                    int id2 = list[i].getSource().getIdFromIndex(index);
                    tracks.add(new Integer(index));
                    tracksID.add(new Integer(id2));
                }
            }
            else
            {
                return new Action[0];
            }
        } // for
        int numTracks2 = tracks.size();
        if (numTracks2 < 2)
        {
            return new Action[0];
        }
        final String trackName = list[0].getSource().getNameScreenName();
        final String trackStoreGateKey = list[0].getSource().getStoreGateKey();
        final int[] trackNumbers = new int[numTracks2]; // indices of tracks
        final int[] trackIDs = new int[numTracks2]; // IDs of tracks
        Iterator<Integer> i = tracks.iterator();
        int n = 0;
        while (i.hasNext())
        {
            trackNumbers[n++] = (i.next()).intValue();
        }
        i = tracksID.iterator();
        n = 0;
        while (i.hasNext())
        {
            trackIDs[n++] = (i.next()).intValue();
        }
        Action[] action = new Action[1]; // only one action will be displayed
        								 // after removing interactive Athena
        action[0] = new AbstractAction("Vertex " + trackName + " " + trackStoreGateKey + " in Atlantis")
        {
            public void actionPerformed(ActionEvent e)
            {
                try
                {
                    createVertexFromTracks(trackNumbers, trackStoreGateKey);
                    // maybe should be all windows
                    ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
                    node.setUserObject(new AList(ARVxData.this, numData - 1));
                }
                catch (Exception ee)
                {
                    // this exception was initially completely ignored (empty
                    // block) - can't actually see the reason why it is thrown
                    // at the end of createVertexFromTracks since it should be
                    // ignored. whether or not the exception occures tells
                    // whether the tracks are coming from the common vertex

                    logger.debug("ARVxData.getActions() exception:  " + ee.getMessage(), ee);
                }
            }
        };

        return action;
    } // getActions() -------------------------------------------------------


    /**
     * The method is called when Remove action is selected after right-click
     * on the list item. There may be vertices coming from different types of
     * tracks, trackType variable assures that the current reconstructed track
     * instance is retrieved and the helix paremeters set on corrent tracks
     * @param index int[]
     */
    @Override
    public void remove(int[] index)
    {
        String key = null;
        ATrackData aRTr = null;
        for (int i = 0; i < index.length; ++i)
        {
            drawFlag.set(index[i], new Boolean(false));
            key = tracksCollection.get(index[i]);
            aRTr = event.getTrackData("InDetTrack", key);
            if (aRTr != null)
            {
                int[] tracksInVertex = (tracksIndex.get(index[i]));
                for (int j = 0; j < tracksInVertex.length; ++j)
                {
                    aRTr.getModifiableHelix(tracksInVertex[j]).setPhiStartByXYPoint(0., 0.);
                }
            } // if
        } // for
    } // remove() -----------------------------------------------------------

    /**
     * if RVx (with numData > 0) is present in this event, cut Tracks
     * forming vertices when tracks are drawn as polylines
     */
    public void cutTracksToRVx(AEvent event)
    {
    	if (!hasConsistentTrackNumbers) return;
        for (int i = 0; i < numData; i++)
        {
            boolean origin = athenaOrigin.get(i).booleanValue();
            if (origin)
            {
                // true - RVx coming from Athena -> cut the track if its
                // storeGateKey is known (was saved into tracksCollection)
                String trackKey = tracksCollection.get(i);
                ATrackData tracks = event.getTrackData("InDetTrack", trackKey);
                if (tracks != null)
                {
                    int[] tracksIdInVertex = (tracksId.get(i));
                    if (tracksIdInVertex.length < 2) //ACH - put some more cuts on this?
                        continue;
                    AHelix[] thisHelGoToFitter = new AHelix[tracksIdInVertex.length];
                    AVertex vertex = vertices.get(i);
                    double[] par = vertex.getPosition();
                    double Vx = par[0];
                    double Vy = par[1];
                    for (int j = 0; j < tracksIdInVertex.length; j++)
                    {
                        int id2 = tracksIdInVertex[j];
                        // if the JiveXML knows that vertex creates X number of
                        // tracks but doesn't know the ID, -1 is put as tracks ID
                        // skip cutting tracks - ID is actually unknown
                        if (id2 > -1)
                        {
                            thisHelGoToFitter[j] = tracks.getModifiableHelix(id2);
                            thisHelGoToFitter[j].setPhiStartByXYPoint(Vx, Vy);
                        }
                    }
                } // if(tracks != null)
            }
        }
    } // cutTracksToRVx() ----------------------------------------------------



    /**
     * Returns the values of primary vertex for the current event using the
     * first vertex in this RVx data if it exists.
     * @return double[3] or null
     */
    public double[] getPrimaryVertex()
    {
        double[] vtx = null; // vertex position (x, y, z order)

        if (!(numData > 0))
        {
            // Check this? If we have no vertices, want to get a primary vertex
            // from somewhere else
            return vtx; // null
        }

        // if datatype contains primVxCand subtag, then take first item
        // flagged 1 (primary vertex candidate) otherwise first item of
        // the whole datatype
        if(primVxCand != null)
        {
            for(int i = 0; i < numData; i++)
            {
                if(primVxCand[i] == 1)
                {
                    vtx = vertices.get(i).getPosition();
                    break;
                }
            }
            if(vtx == null)
            {
                // none of the vetices had flag 1, take the first one anyway
                vtx = vertices.get(0).getPosition();
            }
        }
        else
        {
            // primVxCand is not available, take the first item
            vtx = vertices.get(0).getPosition();
        }

        return vtx;

    } // getPrimaryVertex() -------------------------------------------------



    /**
     *
     * @param index int
     * @return double[]
     * Returns double array with x, y, z positions of the vertex
     */
    public double[] getVertex(int index)
    {
        double[] vtx = null;
        vtx = vertices.get(index).getPosition();
        return vtx;

    } // getVertex() --------------------------------------------------------


} // class ARVxData =========================================================
