
function switcher() {
   this.runs={};
   this.headers=[];
   this.imagesrc=['images/plus.gif','images/minus.gif'];
}

switcher.prototype.init=function() {
   var instanceOf=this;
   this.collectElementbyClass("run")
   for (var i=0; i<this.headers.length; i++) {
      var header = this.headers[i]
      var id = header.id;
      this.runs[id]=new switchlb(header);
      this.runs[id].imagesrc=this.imagesrc;
      this.runs[id].init();
   }
   //document.getElementById("expand-all").onclick=function(){instanceOf.toggleAll("doopen")};
   //document.getElementById("collapse-all").onclick=function(){instanceOf.toggleAll("doclose")};
   //alert("Test");
};

switcher.prototype.collectElementbyClass=function(classname) {
   var classnameRE=new RegExp("(^|\\s+)"+classname+"($|\\s+)", "i");
   var allelements = document.getElementsByTagName("tr");
   for (var i=0; i<allelements.length; i++) {
      var lineEl = allelements[i];
      //if (lineEl.id!="" && typeof lineEl.className=="string" && lineEl.className.search(classnameRE)!=-1)
      if (lineEl.id!="" && typeof lineEl.className=="string" && lineEl.className.match("^"+classname))
         this.headers.push(lineEl);
   }
};

switcher.prototype.toggleAll=function(action) {
   for (var i=0; i<this.headers.length; i++) {
      var id = this.headers[i].id
      this.runs[id].toggle(action);
   }
}
   
function switchlb(header) {
   this.header = header;
   this.pmelem = this.header.getElementsByTagName("td")[0];
   this.status = "open";
   this.imagesrc=['plus.gif','minus.gif'];
};

switchlb.prototype.init=function() {
   var instanceOf=this;
   this.toggle("doclose")
   this.pmelem.onclick=function(){instanceOf.toggle()}
};

switchlb.prototype.toggle=function(action){
   this.collectLbByClassName(this.header.id);
   if(action==undefined) {
      if (this.status=="open") {
         this.contractcontent();
      } else {
         this.expandcontent();
      }
   } else if(action=="doclose") {
      this.contractcontent();
   } else if(action=="doopen") {
      this.expandcontent();
   }
};

switchlb.prototype.contractcontent=function(){
   for(var i=0; i<this.lbs.length; i++) {
      var innercontent=this.lbs[i];
      innercontent.style.visibility="collapse";
   }
   this.pmelem.innerHTML='<img src="'+this.imagesrc[0]+'"/>'
   this.status="closed";
};

switchlb.prototype.expandcontent=function(){
   for(var i=0; i<this.lbs.length; i++) {
      var innercontent=this.lbs[i];
      innercontent.style.visibility="visible";
   }
   this.pmelem.innerHTML='<img src="'+this.imagesrc[1]+'"/>'
   this.status="open";
};

switchlb.prototype.collectLbByClassName=function(classname) {
   var classnameRE=new RegExp("(^|\\s+)"+classname+"($|\\s+)", "i") //regular expression to screen for classname within element
   var allLb = document.getElementsByTagName("tr")
   this.lbs=[]
   for (var i=0; i<allLb.length; i++) {
      var lineEl = allLb[i]
      if (typeof lineEl.className=="string" && lineEl.className.match("^lb-"+classname+"-")) {
         this.lbs.push(lineEl);
      }
   }
}
