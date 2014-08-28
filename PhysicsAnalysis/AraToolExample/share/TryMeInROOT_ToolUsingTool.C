//Run "root -b"
//Copy and paste the following into the command prompt

 TPython::ExecScript("init_root.py");

 tool=new AraToolUsingTool();
 float a=1,b;

 tool->initialize(); //Important
 tool->alsoPerformTask(a,b);

 std::cout<<"Tool Return Result: "<<b<<endl;
 tool->finalize();

