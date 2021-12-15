-- EmbHardw
-- Sébastien Deriaz
-- 01.12.2021
--
-- Grayscale custom instruction
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity grayscale is
    port (
        -- Avalon custom interface signals
        Clk    : in std_logic;
        reset  : in std_logic;
        dataa  : in std_logic_vector(31 downto 0);
        start  : in std_logic;
        result : out std_logic_vector(31 downto 0);
        done   : out std_logic
    );
end grayscale;

architecture comp of grayscale is
    signal R : std_logic_vector(7 downto 0);
    signal G : std_logic_vector(7 downto 0);
    signal B : std_logic_vector(7 downto 0);
begin
    R <= dataa(15 downto 11) & "000";
    G <= dataa(10 downto 5) & "00";
    B <= dataa(4 downto 0) & "000";
    -- Avalon interfacing
    process (Clk, reset)

        variable add : unsigned(31 downto 0);
    begin
        add := (others => '0');
        if reset = '1' then
            result <= (others => '0');
        elsif rising_edge(Clk) then
            if (start = '1') then
                add := resize((unsigned(R) * 30 + unsigned(G) * 59 + unsigned(B) * 11) / 100, 32);
                result <= std_logic_vector(add);
                -- gray = (rgb >> 2) & 0x3E00;   // red
                -- gray += (rgb << 4) & 0x7E00;  // green
                -- gray += (rgb << 8) & 0x1F00;  // blue
                -- gray = gray >> 8;
                -- add := unsigned("00" & R & "00000") + unsigned("00" & G & "000000") + unsigned("00" & B & "000");
                -- result(8 downto 0) <= std_logic_vector(add)(13 downto 5);

                done   <= '1';
            else
                done <= '0';
            end if;
        end if;
    end process;
end comp;