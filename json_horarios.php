<?php
require("phpMQTT.php");

$server = "ifce.sanusb.org";     // change if necessary

$port = 1883;                     // change if necessary

$username = "";                   // set your username

$password = "";                   // set your password

$client_id = "SanUSB.org"; // make sure this is unique for connecting to sever - you could use uniqid()

$porcao = $_GET['porcao'];
$h1 = $_GET['h1'];
$m1 = $_GET['m1'];
$h2 = $_GET['h2'];
$m2 = $_GET['m2'];
$h3 = $_GET['h3'];
$m3 = $_GET['m3'];

if($porcao == "-") {
	$porcao = 3;
}
if($h1 == ""){
	$h1= 100;
}
if($m1==""){
	$m1= 100;
}
if($h2==""){
	$h2= 100;
}
if($m2==""){
	$m2= 100;
}
if($h3==""){
	$h3= 100;
}
if($m3==""){
	$m3= 100;
}

$json_horarios = "{\"porcao\":" . $porcao . ", \"h1\":" . $h1 . ",\"m1\":" . $m1 . ",\"h2\":" . $h2 . ",\"m2\":" . $m2 . ",\"h3\":" . $h3 . ",\"m3\":" . $m3 ."}";


$fl = fopen("view", "w");           //  w trunca no início para curl
$cont = "$json_horarios";
fwrite($fl,$cont);
fclose($fl);

ini_set('default_charset','UTF-8'); // corrigir acentuação dos caracteres no php.
header('Content-Type: text/html; charset=utf-8'); // acentuação dos caracteres no html.

// inicio mqtt

$mqtt = new phpMQTT($server, $port, $client_id);

if ($mqtt->connect(true, NULL, $username, $password)) {

    $mqtt->publish("alimentador", "$json_horarios", 0);

    $mqtt->close();
}
//fim mqtt
echo $json_horarios;
?>
