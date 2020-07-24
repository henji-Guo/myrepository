print("the uart start working at D7(TXD) D8(RXD) ... ")
uart.alt(1)
uart.on("data", "\n",
  function(data)
    print("receive from uart:" .. data)
    if string.find(data,"ON",1)
    then
        --uart.on("data") -- unregister callback function
        m:publish("/topic", "LED_ON", 0, 0)
    elseif string.find(data,"OFF",1)
    then 
        --print("bbb")
        m:publish("/topic", "LED_OFF", 0, 0) 
    else
        m:publish("/topic", data, 0, 0)
    end
end, 0)
    
