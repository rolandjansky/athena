package atlantis.projection;

import atlantis.nge.ANAnimVar;
import atlantis.nge.ANFrameManager;
import atlantis.nge.ANProjectionPhysics;
import atlantis.utils.AMath;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;

/**
 *
 * @author Adam Davison, Mark Stockton
 */
public class AProjectionNPhysics extends AProjectionNGE {

    private ANProjectionPhysics m_p = null;
    private ANFrameManager m_fm = null;
    
    @Override
    public void configureFrame(ANFrameManager fm) {
        m_fm = fm;
        m_p = fm.checkProjectionPhysics();
    }

    @Override
    public String getName() {
        return "NPhysics";
    }

    @Override
    public String getScreenName() {
        return "NPhysics";
    }

    @Override
    public JMenuItem[] getPopupItems() {
        
        JMenuItem phi = new JMenuItem(AMath.ETA + " view");
        phi.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                phiView();
            }
        });
        
        JMenuItem[] i = {
            phi
        };
        return i;
    }

    @Override
    public String getXLabel() {
        return "X";
    }

    @Override
    public String getYLabel() {
        return "Y";
    }

    @Override
    public String getXUnits() {
        return "m";
    }

    @Override
    public String getYUnits() {
        return "m";
    }

    @Override
    public void setScales() {
        
    }

    @Override
    public boolean processLocalCommand(String name) {
        return false;
    }

    public void actionPerformed(ActionEvent e) {
    }
    
    public void phiView() {
        
        ANAnimVar pelev = m_p.getElevVar();
        ANAnimVar pphi = m_p.getPhiVar();
        
        m_fm.getAnimationManager().scheduleAnimation(pelev, 0.0, 1.0, pelev.getValue(), -90.0);
        m_fm.getAnimationManager().scheduleAnimation(pphi, 0.0, 1.0, pphi.getValue(), 0.0);
        
    }

}
