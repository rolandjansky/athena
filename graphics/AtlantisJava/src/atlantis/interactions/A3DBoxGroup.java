package atlantis.interactions;


public class A3DBoxGroup extends AInteractionGroup {

  private A3DBoxInteraction transformInteraction;

  public A3DBoxGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);
    transformInteraction=new A3DBoxInteraction();
    this.addInteraction(transformInteraction);
  }

}
