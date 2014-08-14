package atlantis.projection;

import atlantis.nge.ANFrameManager;
import java.awt.event.ActionEvent;
import javax.swing.JMenuItem;

/**
 *
 * @author Adam Davison
 */
public class AProjectionNYX extends AProjectionNGE {

    public AProjectionNYX() {
        
    }
    
    @Override
    public void configureFrame(ANFrameManager fm) {
        fm.checkProjectionYX();
    }

    @Override
    public String getName() {
        return "NYX";
    }

    @Override
    public String getScreenName() {
        return "NYX";
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
        //
    }

}
