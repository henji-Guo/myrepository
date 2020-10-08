gpio.write(2,gpio.HIGH)


--[[pin = 1
status, temp, humi, temp_dec, humi_dec = dht.read11(pin)
if status == dht.OK then
    -- Integer firmware using this example
    print(string.format("DHT Temperature:%d.%03d;Humidity:%d.%03d\r\n",
          math.floor(temp),
          temp_dec,
          math.floor(humi),
          humi_dec
    ))

    -- Float firmware using this example
    print("DHT Temperature:"..temp..";".."Humidity:"..humi)

elseif status == dht.ERROR_CHECKSUM then
    print( "DHT Checksum error." )
elseif status == dht.ERROR_TIMEOUT then
    print( "DHT timed out." )
end]]--


--[[msg = {}
msg.LED_ON = "sad"
msg.q = "sd"
value = "q"
print(check_table(value,msg))
pcall(check_table(value,msg))
if check_table(value,msg) == true
then 
    print("123")
end

function check_table(value,msg)
for k,v in pairs(msg) do
      if k == value 
      then
          --print(k,v)
          return true
      end
    end
    return false
end]]--
--[[if is_include("LED_ON",msg) == true
then
    if msg.LED_ON == true
    then
        print(1)
        gpio.write(0,gpio.LOW)
    else
        print(2)
        gpio.write(0,gpio.HIGH)
    end
end
]]--

