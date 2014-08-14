package atlantis.projection;

import atlantis.nge.ANFrameManager;
import java.awt.event.ActionEvent;
import javax.swing.JMenuItem;

/**
 *
 * @author Adam Davison
 */
public class AProjectionN3D extends AProjectionNGE {

    @Override
    public void configureFrame(ANFrameManager fm) {
        fm.checkProjection3D();
    }

    @Override
    public String getName() {
        return "N3D";
    }

    @Override
    public String getScreenName() {
        return "N3D";
    }

    @Override
    public JMenuItem[] getPopupItems() {
        return null;
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

}
