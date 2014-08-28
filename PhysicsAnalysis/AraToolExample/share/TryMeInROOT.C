//Run "root -b"
//Copy and paste the following into the command prompt

 TPython::ExecScript("init_root.py");

 tool=new AnExampleAraTool();
 float a=1,b;
 tool->performTask(a,b);

 std::cout<<"Tool Return Result: "<<b<<endl;

