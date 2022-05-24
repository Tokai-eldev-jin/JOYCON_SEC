<!doctype html>
<!--
Copyright 2017-2020 JellyWare Inc. All Rights Reserved.
-->
<html>
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="description" content="BlueJelly">
    <meta name="viewport" content="width=640, maximum-scale=1.0, user-scalable=yes">
    <title>MR Sensor JOYSTICK  DEMO</title>
    <!--<link href="https://fonts.googleapis.com/css?family=Lato:100,300,400,700,900" rel="stylesheet" type="text/css">
    <link rel="stylesheet" href="style.css">-->
    <script type="text/javascript" src="bluejelly.js"></script>
    <script type="text/javascript" src="./smoothie.js"></script>
  </head>

<body>
<div class="container">
    <div class="title margin">
        <font color="orange"> <h4><p id="title">JOY STICK DEMO</p></h4></font>
    </div>

    <div class="contents margin">
        <button id="startNotifications" class="button">Start Notify</button>
        <button id="stopNotifications" class="button">Stop Notify</button>
		<!--<input id="write_value" class="button" value="up" size="20">
        <button id="write" class="button">Write</button>-->　　　　　　　　
        <button id="offset" class="button">OFFSET</button>
        <hr>
        <div id="svg">GRAPH AREA</div>
        <hr>
        <span id="data_text"> </span>
        <span>　　</span>
        <span id="data_text2"> </span>
        <!--<div id="device_name"> </div>
        <div id="uuid_name"> </div>
        
        <div id="status"> </div>-->

    </div>
    <!--<div class="footer margin">
                For more information, see <a href="https://jellyware.jp/kurage" target="_blank">jellyware.jp</a> and <a href="https://github.com/electricbaka/bluejelly" target="_blank">GitHub</a> !
    </div>-->
</div>


<script>
//--------------------------------------------------
//Global変数
//--------------------------------------------------
//BlueJellyのインスタンス生成
const ble = new BlueJelly();
const ble2 = new BlueJelly();

//TimeSeriesのインスタンス生成
const ble_data = new TimeSeries();



let startflag=0;
let value=0;
let value2=0;
let t=0;
let g_width = 800;
let g_heigh = 800;
let i=0;
let offx=-2600;
let offy=-4250;
let rangeX=500;
let rangeY=500;
	
var Xval = new Array(10);
var Yval = new Array(10);
    
for(i=0;i<10;i++){
    Xval[i] = g_width / 2;
    Yval[i] = g_heigh / 2;
}
 
 
 
 


//-------------------------------------------------
//smoothie.js
//-------------------------------------------------
function createTimeline() {
    const chart = new SmoothieChart({
        millisPerPixel: 20,
        grid: {
            fillStyle: '#ff8319',
            strokeStyle: '#ffffff',
            millisPerLine: 800
        },
        maxValue: 5000,
        minValue: 0
    });
    chart.addTimeSeries(ble_data, {
        strokeStyle: 'rgba(255, 255, 255, 1)',
        fillStyle: 'rgba(255, 255, 255, 0.2)',
        lineWidth: 4
    });
    chart.streamTo(document.getElementById("chart"), 500);
}


//--------------------------------------------------
//ロード時の処理
//--------------------------------------------------
window.onload = function () {
  //UUIDの設定
  ble.setUUID("UUID1","dd5f7232-1560-4792-953d-0b2015f15340","8796fa1b-986d-419a-8f84-137710a2354f");
  ble2.setUUID("UUID1","dd5f7232-1560-4792-953d-0b2015f15340","1e630bfc-08ca-44c0-a7c5-58dae380884d");
  //smoothie.js
  //createTimeline();

  main();
};


//--------------------------------------------------
//Scan後の処理
//--------------------------------------------------
ble.onScan = function (deviceName) {
  //document.getElementById('device_name').innerHTML = deviceName;
  document.getElementById('status').innerHTML = "found device!";
}


//--------------------------------------------------
//ConnectGATT後の処理
//--------------------------------------------------
ble.onConnectGATT = function (uuid) {
  console.log('> connected GATT!');

  //document.getElementById('uuid_name').innerHTML = uuid;
  //document.getElementById('status').innerHTML = "connected GATT!";
}


//--------------------------------------------------
//Read後の処理：得られたデータの表示など行う
//--------------------------------------------------
ble.onRead = function (data, uuid){
  //フォーマットに従って値を取得

  let getvalue="";
  for(let i = 0; i < data.byteLength; i++){
    getvalue = getvalue + String.fromCharCode(data.getInt8(i));
  }
  

  //数値化
  let array_atai =getvalue.split(',');
  
  value = Number(array_atai[0]);
  value2 = Number(array_atai[1]);

  //コンソールに値を表示
  //console.log(value+" "+value2);
  
  let str_value="";
  let str_value2="";
  
  if(String(value).length==1) str_value= "000"+value;
  if(String(value).length==2) str_value= "00"+value;
  if(String(value).length==3) str_value= "0"+value;
  if(String(value).length==4) str_value= value;

  if(String(value2).length==1) str_value2= "000"+value2;
  if(String(value2).length==2) str_value2= "00"+value2;
  if(String(value2).length==3) str_value2= "0"+value2;
  if(String(value2).length==4) str_value2= value2;
  
  //HTMLにデータを表示
  document.getElementById('data_text').innerHTML = str_value;
  document.getElementById('data_text2').innerHTML = str_value2;
  //document.getElementById('uuid_name').innerHTML = uuid;
  //document.getElementById('status').innerHTML = "read data"

  //グラフへ反映
  //ble_data.append(new Date().getTime(), value);
  //Create_grapf(value,value2);
  
  startflag=1;
}


//--------------------------------------------------
//Write後の処理
//--------------------------------------------------
ble2.onWrite = function(uuid){
  //document.getElementById('uuid_name').innerHTML = uuid;
  //document.getElementById('status').innerHTML = "written data"
}




//--------------------------------------------------
//Start Notify後の処理
//--------------------------------------------------
ble.onStartNotify = function(uuid){
  console.log('> Start Notify!');

  //document.getElementById('uuid_name').innerHTML = uuid;
  //document.getElementById('status').innerHTML = "started Notify";
}


//--------------------------------------------------
//Stop Notify後の処理
//--------------------------------------------------
ble.onStopNotify = function(uuid){
  console.log('> Stop Notify!');

  //document.getElementById('uuid_name').innerHTML = uuid;
  //document.getElementById('status').innerHTML = "stopped Notify";
}


//-------------------------------------------------
//ボタンが押された時のイベント登録
//--------------------------------------------------
document.getElementById('startNotifications').addEventListener('click', function() {
      ble.startNotify('UUID1');
});

document.getElementById('stopNotifications').addEventListener('click', function() {
      ble.stopNotify('UUID1');
});

/*
document.getElementById('write').addEventListener('click', function() {
  //フォーマットに従って値を変換
  const textEncoder = new TextEncoder();
  const text_data = document.getElementById('write_value').value;
  const text_data_encoded = textEncoder.encode(text_data + '\n');

  //write
  ble2.write('UUID1', text_data_encoded);
});
*/


document.getElementById('offset').addEventListener('click', function() {
  //フォーマットに従って値を変換
  let getx=value;
  let gety=value2;
  
  getx *= g_width;
  getx /= rangeX;
  offx=(g_width/2-getx);
  
  gety *= g_heigh;
  gety /= rangeY;
  offy=(g_heigh/2-gety);
  
  console.log(offx+" "+offy);
  
});













async function main() {
	while(true){
		await wait(10); //
		if(startflag==1) break;
	}
	
	while(true){
	    await wait(100); //
	    Create_grapf();
    }
};



const wait = (ms) => {
  return new Promise((resolve, reject) => {
    setTimeout(resolve, ms);
  });
};



function Create_grapf() {
	
	let Max_val = 5000;
	let Min_val = 0;
	
	var plot_color = new Array('red', 'blue', 'yellow' ,'green');
	
	//document.getElementById("svg").innerHTML =  "hohohohohoh";
	
    let display_text="<svg xmlns='http://www.w3.org/2000/svg' version='1.1' height='" + g_heigh + "' width='" + g_width + "' viewBox='-500 -200 1500 1500' class='SvgFrame'>";
    display_text += "<line x1='0' y1='0' x2='" + g_width + "' y2='0' style='stroke:blue;stroke-width:1' />";
    display_text += "<line x1='0' y1='" + g_heigh + "' x2='" + g_width + "' y2='" + g_heigh + "' style='stroke:blue;stroke-width:1' />";
    display_text += "<line x1='0' y1='0' x2='0' y2='" + g_heigh + "' style='stroke:blue;stroke-width:1' />";
    display_text += "<line x1='" + g_width + "' y1='0' x2='" + g_width + "' y2='" + g_heigh + "' style='stroke:blue;stroke-width:1' />";
    
    for(i=1;i<10;i++){
    	display_text += "<line x1='0' y1='" + g_heigh * i/10 + "' x2='" + g_width + "' y2='" + g_heigh * i/10 + "' style='stroke:cyan;stroke-width:1' />";
    }

    for(i=1;i<10;i++){
    	display_text += "<line x1='" + g_width * i/10 + "' y1='0' x2='" + g_width * i/10 + "' y2='" + g_heigh + "' style='stroke:cyan;stroke-width:1' />";
    }
    
    
    for(i=0;i<11;i++){
    	display_text += "<text x='-30' y=" + (g_heigh * i/10+3) + " font-size='30' stroke='black' text-anchor='end'  stroke-width='0.5'>"+(10-i*2)+"°</text>";
    }

    for(i=0;i<11;i++){
    	display_text += "<text x='" + g_width * i/10 + "' y=" + (g_heigh+60) + " font-size='30' stroke='black' text-anchor='middle'  stroke-width='0.5'>"+(-10+i*2)+"°</text>";
    }    
    
    display_text += "<rect x='" +(g_width-200) +"' y='" + (-100) + "' width='200' height='50' stroke-width='1' stroke='blue' fill='white'/>";
    display_text += "<text x='" + (g_width-190) + "' y='" + (-60) + "' font-size='30' stroke='black' text-anchor='start'  stroke-width='0.5'>角度精度0.1°</text>";
    display_text += "<text x='" + (g_width/2) + "' y='" + (g_heigh+100) + "' font-size='30' stroke='black' text-anchor='middle'  stroke-width='0.5'>X position</text>";
    display_text += "<path id='target_path' d='M -100,"+(g_heigh*6/10)+" L -100,"+(g_heigh*4/10)+" Z'stroke='none' fill='none' />";
    display_text += "<text font-size='30' dy='-10'><textPath xlink:href='#target_path'>Y position</textPath></text>";

    	
	let XX=0;
    let YY=0;
    
    
    
	for(t=0;t<9;t++){
		Xval[t] = Xval[(t+1)];
		Yval[t] = Yval[(t+1)];
	}
        
        
    
    XX=value;
    YY=value2;
    


    
    XX = XX * g_width / rangeX;
    YY = YY * g_heigh / rangeY;
 
	
	XX += offx;
	YY += offy;
	
	if(XX<0) XX=0;
	if(XX>g_width) XX=g_width;
	if(YY<0) YY=0;
	if(YY>g_heigh) YY=g_heigh;
    
    Xval[9] = XX;
    Yval[9] = YY;
    
    
    
    
       
    let display_text2 = display_text;
    for(t=5;t<10;t++){
        display_text2 = display_text2 + "<circle fill='blue' cx='" + Xval[t] + "' cy='" + Yval[t] + "' r='30'></circle>";
    }
    //display_text2 = display_text2 + "<circle fill='blue' cx='" + XX + "' cy='" + YY + "' r='30'></circle>";
    display_text2 = display_text2 + "</svg>";
    
    document.getElementById("svg").innerHTML =  display_text2;


}

</script>
</body>
</html>
