package atlantis.nge;

import atlantis.nge.object.data.ANObjectDataJet;
import atlantis.nge.object.geometry.ANObjectGeomMuon;
import atlantis.nge.object.geometry.ANObjectGeomCalo;
import atlantis.nge.object.data.ANObjectDataCell;
import atlantis.data.ACalorimeterData;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.data.AJetData;
import atlantis.data.ALArData;
import atlantis.geometry.AAtlasDetector;
import atlantis.geometry.ABarrelCalorimeterDetector;
import atlantis.geometry.ABoxDetector;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.geometry.ADetector;
import atlantis.geometry.ADetectors;
import atlantis.geometry.AEndcapCalorimeterDetector;
import atlantis.geometry.AEndcapCryostatDetector;
import atlantis.utils.AMath;

import java.util.ArrayList;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

/**
 * This class represents projections which are a linear transformation on
 * 3d space. These are easy to handle for a variety of reasons
 *
 * @author Adam Davison, Mark Stockton
 */
public abstract class ANLinearProjection extends ANProjection {

    // DEPRECATED
    public ANObjectGeomMuon m_ogm;//muon
    public ANObjectGeomCalo m_ogc;//calo barrel
    public ANObjectGeomCalo m_ogetp;//extended tile +z
    public ANObjectGeomCalo m_ogtsp;//mbts +z
    public ANObjectGeomCalo m_ogecp;//endcap +z
    public ANObjectGeomCalo m_ogetm;//extended tile -z
    public ANObjectGeomCalo m_ogtsm;//mbts -z
    public ANObjectGeomCalo m_ogecm;//endcap -z
    
    public ANObjectDataJet m_jet;//jets
    public ANObjectDataCell m_lar;//lar cells
    
    private static AEventManager eventManager = AEventManager.instance();
    
    /**
     * The contract of applyLinearTransform is that you can assume
     * the matrix stack is in an identity state and you should always apply
     * the transformation in a relative manner, ie glTransform, glRotate
     * nothing like glLoadMatrix or glLoadIdentity
     * 
     * @param drawable
     */
    public abstract void applyLinearTransform(GLAutoDrawable drawable, int w, int h);

    public abstract void drawScene(GLAutoDrawable drawable, ANRenderHints hint);

    // TODO: A lot of this function should probably be broken up into more
    // subclasses to allow this functionality to be selected more
    // granularly
    @Override
    public final void display(GLAutoDrawable drawable, int w, int h, ANRenderHints hint) {
        GL gl = drawable.getGL();
        gl.glMatrixMode(GL.GL_MODELVIEW);
        gl.glLoadIdentity();
        
        gl.glMatrixMode(GL.GL_PROJECTION);
        
        // This is a bit of a hack maybe...
        if (!hint.getPick()) {
            gl.glLoadIdentity();
        }
        
        gl.glEnable(gl.GL_BLEND);
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA);
        gl.glEnable(GL.GL_DEPTH_TEST);

        gl.glPolygonOffset(2.0f, 1.0f);
        gl.glEnable(GL.GL_POLYGON_OFFSET_FILL);

        applyLinearTransform(drawable, w, h);

        ANColor bg = ANColor.getBGColor();

        gl.glClearColor(bg.r, bg.g, bg.b, 1.0f);
        gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT);

        drawScene(drawable, hint);
    }    // Some function that calls applyLinearTransform and extracts the matrix
    // is needed for the transitions but that can come later...
    
    
    // DEPRECATED - to be replaced shortly by ANObjectGenerator
    public final void createGeometry(){
        ArrayList<ABoxDetector> m_boxes = new ArrayList<ABoxDetector>();
        ArrayList<ACalorimeterDetector> m_calo = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_extt = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_mbts = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_endc = new ArrayList<ACalorimeterDetector>();
        
        ADetectors[] ds = AAtlasDetector.getDetector().getDetectors();
        for (int i = 0; i < ds.length; i++) {
            ADetector[] d = ds[i].getDetectors();
            for (int j = 0; j < d.length; j++) {
                if (d[j] instanceof ABoxDetector) {
                    ABoxDetector bd = (ABoxDetector) d[j];
                    //TODO temporary code to show cut away detector
                    //if((bd.getPhi()>1.2 && bd.getPhi()<5.1) || !ACanvas.getCanvas().getPaintingWindow().getProjection().getName().equals("N3D"))
                    m_boxes.add(bd);
                }else if (d[j] instanceof ABarrelCalorimeterDetector) {
                    ACalorimeterDetector cd = (ACalorimeterDetector) d[j];
                    //extended tile only has one end in geom file and not relicated like others
                    if(d[j].getName().equals("Extended TILE")){
                        m_extt.add(cd);
                    }else{
                        m_calo.add(cd);
                    }
                        
                } else if (d[j] instanceof AEndcapCryostatDetector) {
                    //adds MBTS but only has one end in geom file
                    AEndcapCryostatDetector ecd = (AEndcapCryostatDetector) d[j];
                    m_mbts.add(ecd);
                } else if (d[j] instanceof AEndcapCalorimeterDetector) {
                    //adds LAr endcap + HEC but only has one end in geom file
                    AEndcapCalorimeterDetector ecd = (AEndcapCalorimeterDetector) d[j];
                    m_endc.add(ecd);
                    
                }
                
            }
        }
        
        m_ogm = new ANObjectGeomMuon(m_boxes);
        m_ogc = new ANObjectGeomCalo(m_calo, 4, false);
        m_ogetp = new ANObjectGeomCalo(m_extt, 4, false);
        m_ogtsp = new ANObjectGeomCalo(m_mbts, 1, false);
        m_ogecp = new ANObjectGeomCalo(m_endc, 1, false);
        m_ogetm = new ANObjectGeomCalo(m_extt, 4, true);
        m_ogtsm = new ANObjectGeomCalo(m_mbts, 1, true);
        m_ogecm = new ANObjectGeomCalo(m_endc, 1, true);
        

    }
    
    // DEPRECATED - to be replaced shortly by ANObjectGenerator
    public final void createData(){
     
        AEvent ev = eventManager.getCurrentEvent();
        AJetData ajd = ev.getJetData();
        ajd.makeDrawList();
        m_jet = new ANObjectDataJet(ajd);
        
        ALArData ald = ev.getLArData();
        ald.makeDrawList();
        m_lar = new ANObjectDataCell(ald);
        
    }
    
    // DEPRECATED - to be replaced shortly by ANObjectGenerator
    public final void createGeometry(float phi, float eta, float dr){
        ArrayList<ABoxDetector> m_boxes = new ArrayList<ABoxDetector>();
        ArrayList<ACalorimeterDetector> m_calo = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_extt = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_mbts = new ArrayList<ACalorimeterDetector>();
        ArrayList<ACalorimeterDetector> m_endc = new ArrayList<ACalorimeterDetector>();
        
        
        ADetectors[] ds = AAtlasDetector.getDetector().getDetectors();
        for (int i = 0; i < ds.length; i++) {
            ADetector[] d = ds[i].getDetectors();
            for (int j = 0; j < d.length; j++) {
                if (d[j] instanceof ABoxDetector) {
                    ABoxDetector bd = (ABoxDetector) d[j];
                    //code to show cut away detector
                    float etabd = (float) AMath.etaAbs(bd.getZMin(), bd.getRMin());
                    if(AMath.deltaR(phi,eta,(float)bd.getPhi(),etabd)<dr)
                        m_boxes.add(bd);
                }else if (d[j] instanceof ABarrelCalorimeterDetector) {
                    ACalorimeterDetector cd = (ACalorimeterDetector) d[j];
                    //extended tile only has one end in geom file and not relicated like others
                    if(d[j].getName().equals("Extended TILE")){
                        m_extt.add(cd);
                    }else{
                        m_calo.add(cd);
                    }
                        
                } else if (d[j] instanceof AEndcapCryostatDetector) {
                    //adds MBTS but only has one end in geom file
                    AEndcapCryostatDetector ecd = (AEndcapCryostatDetector) d[j];
                    m_mbts.add(ecd);
                } else if (d[j] instanceof AEndcapCalorimeterDetector) {
                    //adds LAr endcap + HEC but only has one end in geom file
                    AEndcapCalorimeterDetector ecd = (AEndcapCalorimeterDetector) d[j];
                    m_endc.add(ecd);
                    
                }
                
            }
        }
        
        m_ogm = new ANObjectGeomMuon(m_boxes);
        m_ogc = new ANObjectGeomCalo(m_calo, 4, false);
        m_ogetp = new ANObjectGeomCalo(m_extt, 4, false);
        m_ogtsp = new ANObjectGeomCalo(m_mbts, 1, false);
        m_ogecp = new ANObjectGeomCalo(m_endc, 1, false);
        m_ogetm = new ANObjectGeomCalo(m_extt, 4, true);
        m_ogtsm = new ANObjectGeomCalo(m_mbts, 1, true);
        m_ogecm = new ANObjectGeomCalo(m_endc, 1, true);
        

    }
    
    public final void createData(float phi, float eta, float dr){
     
        AEvent ev = eventManager.getCurrentEvent();
        AJetData ajd = ev.getJetData();
        ajd.makeDrawList();
        m_jet = new ANObjectDataJet(ajd, phi, eta, dr);

        ALArData ald = ev.getLArData();
        ald.makeDrawList();
        m_lar = new ANObjectDataCell((ACalorimeterData)ald, phi, eta, dr);

    }
}
