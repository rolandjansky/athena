IncludedModernTriggerConfig=False
if not IncludedModernTriggerConfig:
    from TriggerJobOpts.TriggerGetter import TriggerGetter
    g = TriggerGetter()
    g.configure()
    IncludedModernTriggerConfig=True
