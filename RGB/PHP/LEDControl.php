<h1 style="color:green;">  Grant API Project</h1>
<h4> Click the button after making a selection below:</h4>
<form method="post" action="<?php echo $_SERVER['PHP_SELF'];?>">
    Click to turn ON: <input type="submit" name="fname" value="on">
    <p></p>
    Click to turn OFF: <input type="submit" name="fname" value="off">
</form>

<?php
    $var1 = " ";
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $var1= $_POST["fname"];
        if (empty($var1)) {
            $var1=" ";
        } else {
            $var1=$_POST["fname"];
            echo "Last time your clicked was: $var1";
        }
    }
    
    $myfile = fopen("results.txt", "w") or die("Unable to open file!");
    fwrite($myfile, $var1);
    fclose($myfile);
    ?>