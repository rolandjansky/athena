function returnHistory (hisname) {

//Define contents of page
    contents=
    '<body bgcolor="beige">'+
    '<IMG SRC='+
    hisname+
    '><p>'+
    '<A HREF="javascript:window.close()" >'+
    <h2>Close histo</h2> +
    '</A>'

//Create new Window

    options ="toolbar=0,status=0,menubar=0,scrollbars=1,resizable=1";
    newwindow=window.open("","mywindow", options);
    newwindow.document.write(contents);
    newwindow.document.close();

}




