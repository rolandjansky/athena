package atlantis.parameters;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;

/**
 * This class is a GUI representation of a command button.
 * It just generates the corresponding command when pressed.
 */
public class ACommand extends JButton {
  private String command;

  ACommand(String name, String _command, String toolTip) {
    super(name);

    this.command=_command;

    setToolTipText(toolTip);
    addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ACommandProcessor.receive(command);
      }
    });
  }

  public String getName() {
    return getText();
  }

}
