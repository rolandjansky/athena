package atlantis.interactions;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JComboBox;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;

import atlantis.canvas.ACanvas;
import atlantis.parameters.APar;

public class APickGroup extends AInteractionGroup
{
    private APickInteraction pickInteraction;
    private JComboBox mode;
    // the panel that will contain the additional controls for the interactions
    private JPanel additionalControls;

    private final static String HITS = "Event Data";
    private final static String DETECTORS = "Detectors";

    public APickGroup(AInteractionsManager iManager)
    {
        super(AInteractionGroup.CANVAS_GROUP, iManager);
        
        // create the additional controls panel
        additionalControls = new JPanel();

        pickInteraction = new APickInteraction();
        addInteraction(pickInteraction);
        mode = new JComboBox();
        mode.addItem(HITS);
        mode.addItem(DETECTORS);
        if (parameterStore.get("Event", "PickingMode").getI() == 0)
            mode.setSelectedItem(HITS);
        else
            mode.setSelectedItem(DETECTORS);
        mode.addActionListener(new ModeListener());
        additionalControls.add(mode);
    }
    
    /**
     * pick has additional controls, so return true
     */
    public boolean hasAdditionalControls() {
      return true;
    }
    
    /**
     * returns the additional controls
     */
    public JPanel getAdditionalControls() {
      return additionalControls;
    }

    public JMenuItem getPopupItem()
    {
        // generating the menu
        JMenu menu = new JMenu(name);
        ActionListener l = new PopUpListener();

        menu.add(HITS).addActionListener(l);
        menu.add(DETECTORS).addActionListener(l);
        return menu;
    }

    public void connect()
    {
        super.connect();
        if (parameterStore.get("Event", "PickingMode").getI() == 0)
            mode.setSelectedItem(HITS);
        else
            mode.setSelectedItem(DETECTORS);
    }

    class PopUpListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            ACanvas.getCanvas().getCurrentWindow().getInteractionToolBar().setSelectedGroup(name);
            mode.setSelectedItem(e.getActionCommand());
            if (e.getActionCommand().equals(HITS))
                parameterStore.get("Event", "PickingMode").setI(0);
            else if (e.getActionCommand().equals(DETECTORS))
                parameterStore.get("Event", "PickingMode").setI(1);
        }
    }

    class ModeListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            if (mode.getSelectedItem().equals(HITS))
                parameterStore.get("Event", "PickingMode").setI(0);
            else if (mode.getSelectedItem().equals(DETECTORS))
                parameterStore.get("Event", "PickingMode").setI(1);
        }
    }

}
