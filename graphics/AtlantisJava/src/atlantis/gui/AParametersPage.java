package atlantis.gui;

import atlantis.graphics.layout.AFlowLayout;
import atlantis.parameters.ACommand;
import atlantis.parameters.AParametersGroup;
import javax.swing.JScrollPane;
import javax.swing.JPanel;

/**
 * Used to display a table of parameters together with their associated
 * commands in the Parameters Control.
 */
public class AParametersPage extends ALazyPanel {
  private AParametersGroup group;
  AParametersTable pTable;

  public AParametersPage(AParametersGroup group) {
    this.group=group;
  }

  protected void lazyConstructor() {
    setLayout(new AGUILayoutManager());
    setBorder(null);

    add(createCommandsPanel(group), AGUILayoutManager.AVAILABLExPREFERRED);

    pTable=new AParametersTable(group);
    pTable.setName(group.getScreenName());
    add(new JScrollPane(pTable), AGUILayoutManager.AVAILABLExAVAILABLE);
  }

  private JPanel createCommandsPanel(AParametersGroup group) {
    JPanel commandPanel=new JPanel(new AFlowLayout(5, 5));

    commandPanel.setBorder(null);
    ACommand[] commands=group.getCommands();

    for(int j=0; j<commands.length; j++)
      commandPanel.add(commands[j]);

    return commandPanel;
  }

}
