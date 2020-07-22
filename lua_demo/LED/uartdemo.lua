print("the uart start working at D7(TXD) D8(RXD) ... ")
uart.alt(1)
uart.on("data", "\n",
  function(data)
    print("receive from uart:" .. data)
    if string.find(data,"on",1)
    then
        --uart.on("data") -- unregister callback function
        m:publish("/topic", "lua", 0, 0, function(client) print("sent") end)
    elseif string.find(data,"off",1)
    then 
        --print("bbb")
        m:publish("/topic", "lusa", 0, 0, function(client) print("sent") end) 
    else
        print("useless")
    end
end, 0)
    
